#pragma once

#include <SDL.h>
#include <stdexcept>
#include "TaskQueue.hpp"
#include "RuntimeTypes.hpp"
#include "Drawable.hpp"
#include <iostream>

#define BACKGROUND_LAYER 0
#define ANIMATION_LAYER 1
#define INTERACTIVE_LAYER 2

namespace SmallBasic {

template<typename T>
class Atomic {
private:
    std::mutex mutex;
    T value;
public:
    T get() {
        std::unique_lock<std::mutex> lock(this->mutex);
        T value = this->value;
        lock.unlock();
        return value;
    }
    T operator*() {
        return this->get();
    }
    T const& operator=(T const& value) {
        std::unique_lock<std::mutex> lock(this->mutex);
        this->value = value;
        lock.unlock();
        return value;
    }
    Atomic(T value): value(value) {}
    Atomic() {}
};

typedef Atomic<Callback> AtomicCallback;

class RunLoop {
private:
    struct DispatchTarget {
        std::condition_variable condition;
        std::mutex mutex;
        Callback callback;
        bool done = false;
    };
    std::vector<DispatchTarget *> reusable_targets;
    std::queue<DispatchTarget *> dispatch_queue;

    static RunLoop *run_loop;
    std::thread::id main_thread_id;
    std::mutex mutex;
    std::map<int, SDL_Texture *> textures;
    std::map<int, std::queue<Drawable *>> draw_queue;
    SDL_Window *window;
    SDL_Renderer *renderer;
    TaskQueue gui_queue;

    std::string title = "Small Basic Graphics Window";

    bool can_resize = true;
    bool needs_redraw = true;
    
    RunLoop() {
        Uint32 features = SDL_INIT_VIDEO | SDL_INIT_AUDIO;
        if (SDL_Init(features) == -1) { 
            throw std::runtime_error(SDL_GetError());
        }
        background_color = { 255, 255, 255 };
        mouse_x.store(0);
        mouse_y.store(0);
    }

    void gui_callback(Callback cb) {
        if (cb != nullptr) {
            this->gui_queue.push(cb);
        }
    }

    static std::string get_key_name(SDL_Keycode keycode) {
        std::string name = SDL_GetKeyName(keycode);
        if (name.size() == 1 && name[0] >= '0' && name[0] <= '9') {
            name = "D" + name;
        }
        return name;
    }

    SDL_Texture *get_texture(int idx) {
        auto &texture = this->textures[idx];
        if (texture == NULL) {
            int width, height;
            get_window_size(&width, &height);
            texture = SDL_CreateTexture(this->renderer, SDL_GetWindowPixelFormat(window),
                SDL_TEXTUREACCESS_TARGET, width, height);
            if (idx == BACKGROUND_LAYER) {
                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
            }
            else {
                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            }
        }
        return texture;
    }

    void resize_textures() {
        int width, height;
        get_window_size(&width, &height);
        std::lock_guard<std::mutex> guard(mutex);
        for (auto &pair : textures) {
            int idx = pair.first;
            SDL_Texture *texture = pair.second;
            int tw, th;
            SDL_QueryTexture(texture, NULL, NULL, &tw, &th);
            if (tw >= width && th >= height) continue;
            textures[idx] = NULL;
            SDL_Texture *new_texture = get_texture(idx);
            SDL_SetRenderTarget(renderer, new_texture);
            SDL_Rect rect = { .x = 0, .y = 0, .w = tw, .h = th };
            SDL_RenderCopy(renderer, texture, &rect, &rect);
            SDL_DestroyTexture(texture);
        }
        this->needs_redraw = true;
    }

    void refresh_callback() {
        if (this->needs_redraw) {
            this->interrupt_main();
        }
    }

    static Uint32 refresh_callback_sdl(Uint32 interval, void *run_loop_pt) {
        RunLoop *run_loop = reinterpret_cast<RunLoop *>(run_loop_pt);
        run_loop->refresh_callback();
        return interval;
    }

    void flush_draw_queue() {
        for (auto &pair : this->draw_queue) {
            SDL_Texture *texture = get_texture(pair.first);
            SDL_SetRenderTarget(this->renderer, texture);
            auto &queue = pair.second;
            while (queue.size() > 0) {
                //FIXME: memory leak
                queue.front()->render(this->renderer);
                queue.pop();
            }
        }
    }

    void render_window() {
        flush_draw_queue();

        SDL_SetRenderTarget(renderer, NULL);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        auto bg = *background_color;
        SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
        SDL_RenderClear(renderer);
        for (auto &pair : textures) {
            SDL_Texture *texture = pair.second;
            SDL_Rect rect = { .x = 0, .y = 0 };
            SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
            SDL_RenderCopy(renderer, texture, &rect, &rect);
        }
        SDL_RenderPresent(renderer);
        this->needs_redraw = false;
    }
public:
    AtomicCallback on_timer;
    AtomicCallback on_mouse_down;
    AtomicCallback on_mouse_up;
    AtomicCallback on_mouse_move;
    AtomicCallback on_key_up;
    AtomicCallback on_key_down;

    std::atomic<int> mouse_x;
    std::atomic<int> mouse_y;
    Atomic<std::string> last_key;

    Atomic<Color> background_color;

    static RunLoop *current() {
        if (run_loop == nullptr) {
            run_loop = new RunLoop;
        }
        return run_loop;
    }

    bool is_main_thread() {
        return std::this_thread::get_id() == main_thread_id;
    }

    void request_redraw() {
        std::lock_guard<std::mutex> guard(mutex);
        needs_redraw = true;
    }

    void initialize_window() {
        if (this->window != NULL) {
            return;
        }
        dispatch_main_sync([this]() {
            if (this->window != NULL) {
                return;
            }
            std::unique_lock<std::mutex> lock(mutex);
            auto title = this->title;
            lock.unlock();
            this->window = SDL_CreateWindow(title.c_str(),
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 400,
                SDL_WINDOW_RESIZABLE);
            this->renderer = SDL_CreateRenderer(this->window, -1,
                SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
            //FIXME: Does not handle screen changes
            SDL_DisplayMode mode;
            SDL_GetDesktopDisplayMode(0, &mode);
            int refresh = mode.refresh_rate;
            if (refresh <= 0) {
                refresh = 60;
            }

            SDL_AddTimer(1000 / 60, RunLoop::refresh_callback_sdl,
                reinterpret_cast<void *>(this));
        });
    }

    void dispatch_main_sync(Callback cb) {
        if (is_main_thread()) {
            cb();
            return;
        }

        std::unique_lock<std::mutex> lock(mutex);
        DispatchTarget *target;
        if (reusable_targets.size() > 0) {
            target = reusable_targets[reusable_targets.size()-1];
            reusable_targets.pop_back();
            target->done = false;
        }
        else {
            target = new DispatchTarget;
        }
        dispatch_queue.push(target);
        target->callback = cb;
        lock.unlock();

        std::unique_lock<std::mutex> dispatch_lock(target->mutex);
        interrupt_main();
        bool *done = &target->done;
        target->condition.wait(dispatch_lock, [done]() { return *done; });
        
        lock.lock();
        reusable_targets.push_back(target);
        lock.unlock();
    }

    void interrupt_main() {
        SDL_Event event;
        SDL_memset(&event, 0, sizeof(event));
        event.type = SDL_USEREVENT;
        SDL_PushEvent(&event);
    }

    void show_message(std::string const& title, std::string const& message) {
        dispatch_main_sync([this, title, message]() {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                title.c_str(), message.c_str(), this->window);
        });
    }

    void set_window_title(std::string const& title) {
        dispatch_main_sync([this, title]() {
            initialize_window();
            std::unique_lock<std::mutex> lock(mutex);
            this->title = title;
            SDL_SetWindowTitle(window, title.c_str());
            lock.unlock();
        });
    }

    std::string get_window_title() {
        std::lock_guard<std::mutex> lock(mutex);
        return this->title;
    }

    void queue_draw(int idx, std::vector<Drawable *> drawables) {
        if (drawables.size() == 0) {
            throw std::invalid_argument("drawables.size() == 0");
        }
        this->initialize_window();
        std::lock_guard<std::mutex> guard(mutex);
        auto &queue = draw_queue[idx];
        for (Drawable *drawable : drawables) {
            auto clear = dynamic_cast<DrawableClear *>(drawable);
            if (clear != nullptr) {
                queue = std::queue<Drawable *>();
            }
            queue.push(drawable);
        }
        this->needs_redraw = true;
    }

    void queue_draw(int idx, Drawable *drawable) {
        queue_draw(idx, std::vector<Drawable *>({ drawable }));
    }

    void get_window_size(int *width, int *height) {
        dispatch_main_sync([this, width, height]() {
            initialize_window();
            SDL_GetWindowSize(this->window, width, height);
        });
    }

    void set_window_size(int width, int height) {
        dispatch_main_sync([this, width, height]() {
            initialize_window();
            SDL_SetWindowSize(window, width, height);
            resize_textures();
        });
    }

    void set_can_resize(bool can_resize) {
        std::unique_lock<std::mutex> lock(mutex);
        if (this->can_resize == can_resize) {
            return;
        }
        lock.unlock();
        dispatch_main_sync([this, can_resize]() {
            SDL_SetWindowResizable(window, can_resize ? SDL_TRUE : SDL_FALSE);
            this->can_resize = can_resize;
        });
    }

    bool get_can_resize() {
        std::lock_guard<std::mutex> guard(mutex);
        return this->can_resize;
    }

    void get_desktop_size(int *width, int *height) {
        dispatch_main_sync([width, height]{
            SDL_DisplayMode mode;
            SDL_GetDesktopDisplayMode(0, &mode);
            if (width != NULL) {
                *width = mode.w;
            }
            if (height != NULL) {
                *height = mode.h;
            }
        });
    }

    void get_window_position(int *x, int *y) {
        dispatch_main_sync([this, x, y]{
            SDL_GetWindowPosition(window, x, y);
        });
    }

    void set_window_position(int x, int y) {
        dispatch_main_sync([this, x, y]{
            SDL_SetWindowPosition(window, x, y);
        });
    }

    void hide_window() {
        dispatch_main_sync([this]() {
            initialize_window();
            SDL_HideWindow(window);
        });
    }

    void show_window() {
        dispatch_main_sync([this]() {
            initialize_window();
            SDL_ShowWindow(window);
        });
    }

    void handle_event(SDL_Event const& ev, bool &stop) {
        switch (ev.type) {
            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                bool down = ev.type == SDL_KEYDOWN;
                auto cb = down ? *(this->on_key_down) : *(this->on_key_up);
                std::string key_name = get_key_name(ev.key.keysym.sym);
                gui_callback([this, key_name, cb]() {
                    this->last_key = key_name;
                    if (cb != nullptr) {
                        cb();
                    }
                });
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP: {
                bool down = ev.type == SDL_MOUSEBUTTONDOWN;
                auto cb = down ? *(this->on_mouse_down) : *(this->on_mouse_up);
                gui_callback([cb]() {
                    if (cb != nullptr) {
                        cb();
                    }
                });
                break;
            }
            case SDL_MOUSEMOTION: {
                auto cb = *(this->on_mouse_move);
                int mouse_x = ev.motion.x;
                int mouse_y = ev.motion.y;

                gui_callback([this, cb, mouse_x, mouse_y]() {
                    this->mouse_x = mouse_x;
                    this->mouse_y = mouse_y;
                    if (cb != nullptr) {
                        cb();
                    }
                });
                break;
            }
            case SDL_WINDOWEVENT: {
                if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    resize_textures();
                }
                break;
            }
            case SDL_USEREVENT: {
                std::unique_lock<std::mutex> lock(mutex);
                while (dispatch_queue.size() > 0) {
                    // Get next dispatch target
                    auto target = dispatch_queue.front();
                    std::unique_lock<std::mutex> dispatch_lock(target->mutex);

                    // Unlock run loop lock to allow callback to call
                    // run loop methods (such as request_renderer)
                    lock.unlock();

                    // Dispatch
                    target->callback();

                    // Reacquire run loop lock to pop target from queue
                    lock.lock();

                    // Notify calling thread to continue execution
                    dispatch_lock.unlock();
                    target->done = true;
                    target->condition.notify_one();

                    // Pop dispatch target
                    dispatch_queue.pop();
                }
                if (needs_redraw) {
                    render_window();
                }
                break;
            }
            case SDL_QUIT:
                stop = true;
                break;
            default:
                break;
        }
    }

    void run(Callback entry_point) {
        main_thread_id = std::this_thread::get_id();
        std::thread program_thread(entry_point);

        SDL_Event ev;
        interrupt_main();

        bool stop = false;
        while (SDL_WaitEvent(&ev) != 0) {
            handle_event(ev, stop);
            if (stop) break;
        }
        
        std::cerr << "cannot exit cleanly, calling abort()" << std::endl;
        abort();
    }
};

}