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
    SDL_Window *window;
    SDL_Renderer *renderer;
    TaskQueue gui_queue;

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
    void gui_callback(AtomicCallback &func) {
        auto cb = *func;
        gui_callback(cb);
    }

    static std::string get_key_name(SDL_Keycode keycode) {
        std::string name = SDL_GetKeyName(keycode);
        if (name[0] >= '0' && name[0] <= '9') {
            name = "D" + name;
        }
        return name;
    }

    SDL_Texture *get_texture(int idx) {
        auto &texture = this->textures[idx];
        if (texture == NULL) {
            //FIXME: constant texture size
            texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGBA8888,
                SDL_TEXTUREACCESS_TARGET, 800, 640);
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        }
        return texture;
    }

    void render_window() {
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

    void initialize_window() {
        std::unique_lock<std::mutex> lock(mutex);
        if (this->window != NULL) {
            return;
        }
        lock.unlock();
        dispatch_main_sync([this]() {
            this->window = SDL_CreateWindow("Small Basic Graphics Window",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 400, 0);
            this->renderer = SDL_CreateRenderer(this->window, -1,
                SDL_RENDERER_ACCELERATED);
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

    void request_redraw() {
        std::unique_lock<std::mutex> lock(mutex);
        needs_redraw = true;
        lock.unlock();
        interrupt_main();
    }

    void interrupt_main() {
        SDL_Event event;
        SDL_memset(&event, 0, sizeof(event));
        event.type = SDL_USEREVENT;
        SDL_PushEvent(&event);
    }

    void set_window_title(std::string const& title) {
        dispatch_main_sync([this, title]() {
            initialize_window();
            SDL_SetWindowTitle(window, title.c_str());
        });
    }

    std::string get_window_title() {
        std::string title;
        dispatch_main_sync([this, &title]() {
            initialize_window();
            title = SDL_GetWindowTitle(window);
        });
        return title;
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

    void request_draw(int idx, Drawable const& drawable) {
        request_renderer(idx, [&drawable](SDL_Renderer *renderer) {
            drawable.render(renderer);
        });
    }

    void request_renderer(int idx, std::function<void(SDL_Renderer *)> cb) {
        dispatch_main_sync([this, idx, cb]() {
            this->initialize_window();
            std::unique_lock<std::mutex> lock(mutex);
            SDL_Texture *texture = get_texture(idx);
            SDL_SetRenderTarget(this->renderer, texture);
            this->needs_redraw = true;
            cb(this->renderer);
        });
    }

    void handle_event(SDL_Event const& ev, bool &stop) {
        switch (ev.type) {
            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                bool down = ev.type == SDL_KEYDOWN;
                auto cb = down ? *(this->on_key_down) : *(this->on_key_up);
                if (cb != nullptr) {
                    std::string key_name = get_key_name(ev.key.keysym.sym);
                    gui_callback([this, key_name, cb]() {
                        this->last_key = key_name;
                        cb();
                    });
                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP: {
                bool down = ev.type == SDL_MOUSEBUTTONDOWN;
                auto cb = down ? *(this->on_mouse_down) : *(this->on_mouse_up);
                if (cb != nullptr) {
                    gui_callback([cb]() {
                        cb();
                    });
                }
                break;
            }
            case SDL_MOUSEMOTION: {
                auto cb = *(this->on_mouse_move);
                if (cb != nullptr) {
                    int mouse_x = ev.motion.x;
                    int mouse_y = ev.motion.y;

                    gui_callback([this, cb, mouse_x, mouse_y]() {
                        this->mouse_x = mouse_x;
                        this->mouse_y = mouse_y;
                        cb();
                    });
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
        while (program_thread.joinable()) {
            bool stop = false;
            while (SDL_WaitEvent(&ev) != 0) {
                handle_event(ev, stop);
                if (stop) break;
            }
        }
    }
};

}