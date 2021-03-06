#ifndef ENGINE_HPP_INCLUDED
#define ENGINE_HPP_INCLUDED

/* Including all the needed libraries (GLEW, SDL2, SDL2_image, SDL2_mixer(if "Mixer" defined), GLM)*/
#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#ifdef Mixer
#include <SDL2/SDL_mixer.h>
#endif // Mixer

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <string>
#include <fstream>
#include <strstream>

/* Error checking. (A little addition of mine is that little if statement at the end. You can use it if you want :D) */
#define VEN_IFFUCKEDUP(a_Happened, a_Error) bool f_Happened = a_Happened; if(f_Happened){std::cerr << "VEN_IFFUCKEDUP Error: " << a_Error << std::endl;} if(f_Happened)
/* The same error checking thing, but you can't use the if at the end, as it is already used to make an assertion. */
#define VEN_IFFUCKEDUPBREAK(a_Happened, a_Error) VEN_IFFUCKEDUP(a_Happened, a_Error) { assert(false); }

/* Namespace in which everything will be */
namespace Venum {

/* Function used for loading file content as a string */
std::string LoadStringFromFile(const std::string& a_Filename) {
    /* Creating a filestream and opening a file with it */
    std::ifstream f_File(a_Filename);

    /* If file has opened correctly, load a string from it */
    if(f_File.good()) {
        /* The result string */
        std::string f_Result = "";

        /* Line string - The file is being read line by line, so we have to loop through it */
        std::string f_Line;
        while(std::getline(f_File, f_Line)) {
            /* Adding the lines content to the result string */
            f_Result += f_Line;
            /* Adding an end-line to the result string */
            f_Result += '\n';
        }
        #ifdef VENUM_DEBUG_EXTREME
        std::cout << "Loaded file: '" << a_Filename << "' Using 'LoadStringFromFile' Function! " << std::endl;
        #endif // VENUM_DEBUG_EXTREME

        /* Returning the result string */
        return f_Result;
    }
    else {
        /* If the file hasn't opened correctly, state an error and return nothing */
        std::cerr << "Unable to load file from string: '" << a_Filename << "'!" << std::endl;
        return "";
    }
}
/* Function used to map a value in one range to a value in another range */
template<typename T> T Map(T a_Value, T a_Start1, T a_Stop1, T a_Start2, T a_Stop2) {
	return ((a_Value - a_Start1) / (a_Stop1 - a_Start1)) * (a_Stop2 - a_Start2) + a_Start2;
}
/* Function used to constrain a value to some range */
template<typename T> T Constrain(T a_Value, T a_Start, T a_Stop) {
    if(a_Value < a_Start) {
        return a_Start;
    }
    else if(a_Value > a_Stop) {
        return a_Stop;
    }
    else {
        return a_Value;
    }
}

/* Class made for windowing and event handling */
class Window {
    private:
        /* Class for getting the keyboards keys state */
        class KeyboardClass {
            public:
                /* Empty constructor */
                KeyboardClass() {
                    f_Keystate = SDL_GetKeyboardState(NULL);
                }
                /* Empty Destructor */
                ~KeyboardClass() {

                }

                /* Function for getting the keys state */
                bool GetKeyState(Uint32 a_ScanCode) const {
                    return f_Keystate[a_ScanCode];
                }

            protected:
                const Uint8* f_Keystate;

                friend class Window;
        };
        /* Class for getting the mouses position, buttons state, visibility, wheels rotation, and setting position and visibility */
        class MouseClass {
            public:
                /* Empty constructor */
                MouseClass() {
                    m_WheelRotation = 0;
                    m_Window = nullptr;
                }
                /* Empty Destructor */
                ~MouseClass() {

                }

                /* Function for getting the mouse's position */
                glm::vec2 GetPosition() {
                    int f_MousePosition[2];
                    SDL_GetMouseState(&f_MousePosition[0], &f_MousePosition[1]);
                    return glm::vec2(f_MousePosition[0], f_MousePosition[1]);
                }
                /* Function for getting the mouse's wheel's */
                int GetWheelRotation() {
                    return m_WheelRotation;
                }
                /* Function for getting the cursor's visibility */
                bool GetVisibility() {
                    return SDL_ShowCursor(-1);
                }
                /* Function for getting the button's state */
                bool GetButtonDown(unsigned int a_Button) {
                    int f_MousePosition[2];
                    Uint32 f_State = SDL_GetMouseState(&f_MousePosition[0], &f_MousePosition[1]);
                    return (f_State == a_Button);
                }

                /* Function for setting the mouse's position */
                void SetPosition(glm::vec2 a_Position) {
                    SDL_WarpMouseInWindow(m_Window->GetWindow(), a_Position.x, a_Position.y);
                }
                /* Function for setting the cursor's visibility */
                void SetVisibility(bool a_Visible) {
                    SDL_ShowCursor(a_Visible);
                }

            protected:
                Window* m_Window;
                int m_WheelRotation;

                friend class Window;
        };

    public:
        /* Empty constructor, just in case we would want to create window later than creating the object itself */
        Window() {
            m_Running = false;
            m_Created = false;
        }
        /* Constructor calling Create() function. Used if we want to create the window when we create the object. Automatically calls VEN_IFFUCKEDUPASSERT() also, just to be sure the application works properly */
        Window(const std::string& a_Title, unsigned int a_Width = 640, unsigned int a_Height = 360, unsigned int a_Flags = 0) {
            VEN_IFFUCKEDUPBREAK(!Create(a_Title, a_Width, a_Height, a_Flags), "Fockin window ain't opening!");
        }
        /* Destructor, Deletes the context and destroys the window */
        ~Window() {
            if(m_Created) {
                SDL_GL_DeleteContext(m_Context);
                SDL_DestroyWindow(m_Window);

                #ifdef VENUM_DEBUG_EXTREME
                std::cout << "Window Destructed! " << std::endl;
                #endif // VENUM_DEBUG_EXTREME
            }
        }

        /* Create function, used to create the window. Creates the window and the context, also sets the OpenGL attributes. If something went wrong it returns false, otherwise returns true */
        bool Create(const std::string& a_Title, unsigned int a_Width = 640, unsigned int a_Height = 360, unsigned int a_Flags = 0) {
            if(!m_Created) {
                /* Setting Running variable to true, as the window should run now */
                m_Running = false;
                /* Setting Created variable to true, as the window has created */
                m_Created = false;
                /* Creating the window */
                m_Window = SDL_CreateWindow(a_Title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, a_Width, a_Height, SDL_WINDOW_OPENGL | a_Flags);
                if(m_Window == NULL) {
                    /* If couldn't create window, give out an error, return false */
                    std::cerr << "Can't create window!" << std::endl;
                    return false;
                }
                /* Creating the context */
                m_Context = SDL_GL_CreateContext(m_Window);
                if(m_Context == NULL) {
                    /* If couldn't create context, give out an error, return false */
                    std::cerr << "Can't create context!" << std::endl;
                    return false;
                }

                /* Setting all the needed attributes */
                SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
                SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
                SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
                SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
                SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

                SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
                SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

                SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

                /* Initializing OpenGL */
                glewExperimental = GL_TRUE;
                if(glewInit() != GLEW_OK) {
                    /* If couldn't initialize, give out an error, return false */
                    std::cerr << "Can't initialize GLEW!" << std::endl;
                    return false;
                }

                #ifdef VENUM_DEBUG_EXTREME
                std::cout << "Window Created! " << std::endl;
                #endif // VENUM_DEBUG_EXTREME

                /* Sets the mouses window pointer component to this window */
                Mouse.m_Window = this;
                /* If everything went all right set Running and Created to true and return true */
                m_Running = true;
                m_Created = true;
                return true;
            }
            else {
                return false;
            }
        }

        /* Returns Running variable */
        bool Running() const {
            return m_Running;
        }
        /* Sets running to false */
        void Close() {
            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "Window Closed! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME
            m_Running = false;
        }

        /* Closes the window if the event for it has been polled */
        bool PollEvent() {
            SDL_Event f_Event;
            bool f_EventHappened = false;
            Mouse.m_WheelRotation = 0;
            /* Loops through every event that has been polled */
            while(SDL_PollEvent(&f_Event)) {
                /* If the event's type is quit request then close the window */
                if(f_Event.type == SDL_QUIT) {
                    Close();
                }
                /* If the event's type is mouse wheel rotation request then change mouse wheel rotation in Mouse object */
                else if(f_Event.type == SDL_MOUSEWHEEL) {
                    Mouse.m_WheelRotation += f_Event.wheel.y;
                }
                /* If any event has been polled, set the return variable (EventHappened) to true, otherwise it is false */
                f_EventHappened = true;
            }

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "Event Polled! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME

            /* Return the return variable */
            return f_EventHappened;
        }

        /* Function for swapping the main FrameBuffer with windows frame buffer */
        void SwapBuffers() {
            SDL_GL_SwapWindow(m_Window);

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "Window Buffer and Main Buffer Swapped! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME
        }

        /* Fuction for getting the width of the window */
        unsigned int GetWidth() const {
            int f_Width;
            int f_Height;
            SDL_GetWindowSize(m_Window, &f_Width, &f_Height);
            return static_cast<unsigned int>(f_Width);
        }
        /* Function for getting the height of the window */
        unsigned int GetHeight() const {
            int f_Width;
            int f_Height;
            SDL_GetWindowSize(m_Window, &f_Width, &f_Height);
            return static_cast<unsigned int>(f_Height);
        }
        /* Function for getting the size of the window */
        glm::uvec2 GetSize() const {
            return glm::uvec2(GetWidth(), GetHeight());
        }

        /* Fuction for getting the window */
        SDL_Window* GetWindow() const {
            return m_Window;
        }

    public:
        MouseClass Mouse;
        KeyboardClass Keyboard;

    private:
        SDL_Window* m_Window;
        SDL_GLContext m_Context;
        bool m_Running;
        bool m_Created;
};

/* Class for real time timing :p */
class Timer {
    public:
        /* Empty constructor */
        Timer() {

        }
        /* Empty destructor */
        ~Timer() {

        }

        /* Function for reseting the timer */
        void Start() {
            m_StartTicks = SDL_GetTicks();
        }
        /* Function for ending the timer */
        void End() {
            m_ElapsedTicks = SDL_GetTicks() - m_StartTicks;
            m_DeltaTime = (float)m_ElapsedTicks * 0.001f;
        }

        /* Function for getting how much seconds elapsed between start and end */
        float GetDeltaTime() {
            return m_DeltaTime;
        }

    private:
        unsigned int m_StartTicks;
        unsigned int m_ElapsedTicks;
        float m_DeltaTime;
};
/* Class for limiting the fps */
class FPSLimiter {
    public:
        /* Constructor assigning frame rate limit */
        FPSLimiter(unsigned int a_FPS = 60) {
            SetFPSLimit(a_FPS);
        }
        /* Empty destructor */
        ~FPSLimiter() {

        }

        /* Start of the frame */
        void UpdateStart() {
            m_Cycle = SDL_GetTicks();
        }
        /* End of the frame - delaying the next frame if the frame was too fast */
        void UpdateEnd() {
            unsigned int f_CycleEnd = SDL_GetTicks();
            unsigned int f_CycleDifference = f_CycleEnd - m_Cycle;
            float f_Delay = 1000.0f / m_FPSLimit - f_CycleDifference;
            if(f_Delay > 0.0f) {
                SDL_Delay(f_Delay);
            }
        }

        /* Function for setting the frame rate limit */
        void SetFPSLimit(unsigned int a_FPS) {
            m_FPSLimit = a_FPS;
        }

    private:
        unsigned int m_Cycle;
        unsigned int m_FPSLimit;
};

/* Only if "Mixer" was defined */
#ifdef Mixer
/* Music class, only one can play at a time */
class Music {
    public:
        Music() {
            m_Sample = NULL;
        }
        Music(const std::string& a_Filename) {
            m_Sample = NULL;
            LoadMusicFromFile(a_Filename);
        }
        ~Music() {
            Mix_FreeMusic(m_Sample);
        }

        void LoadMusicFromFile(const std::string& a_Filename) {
            if(m_Sample != NULL) {
                Mix_FreeMusic(m_Sample);
            }
            m_Sample = Mix_LoadMUS(a_Filename.c_str());
        }
        void Play(unsigned int a_Loops = 0) {
            Mix_PlayMusic(m_Sample, a_Loops);
        }
        void Pause() {
            Mix_PauseMusic();
        }
        void Resume() {
            Mix_ResumeMusic();
        }
        void Stop() {
            Mix_HaltMusic();
        }

        bool IsPlayed() const {
            return Mix_PlayingMusic();
        }
        bool IsPaused() const {
            return Mix_PausedMusic();
        }
        bool IsStopped() const {
            return !IsPlayed();
        }
        int GetVolume() const {
            return Mix_VolumeMusic(-1);
        }

        void SetVolume(int a_Volume) {
            Mix_VolumeMusic(a_Volume);
        }

    private:
        Mix_Music* m_Sample;
};
/* Sound class, a lot can play at once */
class Sound {
    public:
        Sound(int a_Channel = 0) {
            m_Sample = NULL;
            m_Channel = a_Channel;
        }
        Sound(const std::string& a_Filename, int a_Channel = 0) {
            m_Sample = NULL;
            LoadSoundFromFile(a_Filename);
            m_Channel = a_Channel;
        }
        ~Sound() {
            Mix_FreeChunk(m_Sample);
        }

        void LoadSoundFromFile(const std::string& a_Filename) {
            if(m_Sample != NULL) {
                Mix_FreeChunk(m_Sample);
            }
            m_Sample = Mix_LoadWAV(a_Filename.c_str());
        }
        void Play(unsigned int a_Loops = 0) {
            Mix_PlayChannel(m_Channel, m_Sample, a_Loops);
        }
        void Pause() {
            Mix_Pause(m_Channel);
        }
        void Resume() {
            Mix_Resume(m_Channel);
        }
        void Stop() {
            Mix_HaltChannel(m_Channel);
        }

        bool IsPlayed() const {
            return Mix_Playing(m_Channel);
        }
        bool IsPaused() const {
            return Mix_Paused(m_Channel);
        }
        bool IsStopped() const {
            return !IsPlayed();
        }
        int GetVolume() const {
            return Mix_Volume(m_Channel, -1);
        }
        int GetChannel() const {
            return m_Channel;
        }

        void SetVolume(int a_Volume) {
            Mix_Volume(m_Channel, a_Volume);
        }
        void SetChannel(int a_Channel) {
            m_Channel = a_Channel;
        }

    private:
        Mix_Chunk* m_Sample;
        int m_Channel;
};
#endif // Mixer

/* Transform class - for objects transformations (position, rotation and scale) */
class Transform3D {
    public:
        /* Constructor taking in a position, rotation and scale */
        Transform3D(glm::vec3 a_Position = glm::vec3(0.0f), glm::vec3 a_Rotation = glm::vec3(0.0f), glm::vec3 a_Scale = glm::vec3(1.0f)) {
            Position = a_Position;
            Rotation = a_Rotation;
            Scale = a_Scale;
        }
        /* Copy constructor */
        Transform3D(const Transform3D& a_Other) {
            *this = a_Other;
        }
        /* Empty destructor */
        ~Transform3D() {

        }

        /* Function for getting the rotation of a point in 2D coordinates */
        static float PointsRotation2D(glm::vec2 a_Point) {
            glm::vec2 f_Point = glm::normalize(a_Point);
            return (a_Point != glm::vec2(0.0f) ? glm::degrees(std::atan2(f_Point.y, f_Point.x)) : 0.0f);
        }
        /* Function for getting the rotation of a point in 3D coordinates */
        static glm::vec3 PointsRotation3D(glm::vec3 a_Point) {
            glm::vec3 f_Point = glm::normalize(a_Point);
            return (a_Point != glm::vec3(0.0f, 1.0f, 0.0f) ? glm::degrees(glm::eulerAngles(glm::quatLookAt(f_Point, glm::vec3(0.0f, 1.0f, 0.0f)))) : glm::vec3(0.0f));
        }

        /* Function for getting the translation matrix */
        glm::mat4 GetPositionMatrix() const {
            return glm::translate(glm::mat4(1.0f), Position);
        }
        /* Function for getting the rotation matrix */
        glm::mat4 GetRotationMatrix() const {
            /* We are using quaternions for it, as they are a lot simpler to use compared to 3 different matrices that have to be multiplied :D */
            return glm::toMat4(glm::quat(glm::radians(Rotation)));
        }
        /* Function for getting the scale matrix */
        glm::mat4 GetScaleMatrix() const {
            return glm::scale(glm::mat4(1.0f), Scale);
        }

        /* Function for getting the full model matrix */
        glm::mat4 GetMatrix() const {
            /* We are just simply multiplying translation, rotation and scale matrix */
            return GetPositionMatrix() * GetRotationMatrix() * GetScaleMatrix();
        }

        /* The equal sign operator, used to copy contents of the other transform to that one */
        Transform3D& operator=(const Transform3D& a_Other) {
            Position = a_Other.Position;
            Rotation = a_Other.Rotation;
            Scale = a_Other.Scale;
            return *this;
        }

    public:
        glm::vec3 Position, Rotation, Scale;
};
enum class CameraType {
    None = 0,
    Camera3D = 1,
    Camera2D = 2,
};
class Camera {
    public:
        Camera(Transform3D a_Transform = Transform3D()) {
            Transform = a_Transform;
        }
        glm::vec3 GetForward() const {
            return Transform.GetRotationMatrix() * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
        }
        glm::vec3 GetBackward() const {
            return Transform.GetRotationMatrix() * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        }
        glm::vec3 GetUpside() const {
            return Transform.GetRotationMatrix() * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        }
        glm::vec3 GetDownside() const {
            return Transform.GetRotationMatrix() * glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
        }
        glm::vec3 GetLeft() const {
            return Transform.GetRotationMatrix() * glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f);
        }
        glm::vec3 GetRight() const {
            return Transform.GetRotationMatrix() * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        }

        virtual glm::mat4 GetMatrix() const {
            return glm::mat4(1.0f);
        }

        virtual CameraType GetType() const {
            return CameraType::None;
        }

    public:
        Transform3D Transform;
};
class Camera3D: public Camera {
    public:
        Camera3D(const Transform3D& a_Transform = Transform3D(), float a_FOV = 90.0f, float a_Near = 0.01f, float a_Far = 1000.0f, float a_Aspect = 16.0f / 9.0f) {
            Transform = a_Transform;
            Aspect = Aspect;
            FOV = a_FOV;
            Near = a_Near;
            Far = a_Far;
        }
        Camera3D(const Camera3D& a_Other) {
            *this = a_Other;
        }
        ~Camera3D() {

        }

        glm::mat4 GetMatrix() const {
            return glm::perspective(glm::radians(FOV), Aspect, Near, Far) * glm::inverse(Transform.GetMatrix());
        }

        CameraType GetType() const {
            return CameraType::Camera3D;
        }

        Camera3D& operator=(const Camera3D& a_Other) {
            Transform = a_Other.Transform;
            Aspect = a_Other.Aspect;
            FOV = a_Other.FOV;
            Near = a_Other.Near;
            Far = a_Other.Far;
            return *this;
        }

    public:
        float FOV;
        float Near, Far;
        float Aspect;
};
class Camera2D: public Camera {
    public:
        Camera2D(Transform3D a_Transform = Transform3D(), float a_Left = -1.0f, float a_Down = -1.0f, float a_Right = 1.0f, float a_Top = 1.0f, float a_Near = 0.0f, float a_Far = 1000.0f) {
            Transform = a_Transform;
            Left = a_Left;
            Down = a_Down;
            Right = a_Right;
            Top = a_Top;
            Near = a_Near;
            Far = a_Far;
        }
        Camera2D(const Camera2D& a_Other) {
            *this = a_Other;
        }
        ~Camera2D() {

        }

        glm::mat4 GetMatrix() const {
            return glm::ortho(Left, Right, Down, Top, Near, Far) * glm::inverse(Transform.GetMatrix());
        }

        CameraType GetType() const {
            return CameraType::Camera2D;
        }

        Camera2D& operator=(const Camera2D& a_Other) {
            Transform = a_Other.Transform;
            Left = a_Other.Left;
            Down = a_Other.Down;
            Right = a_Other.Right;
            Top = a_Other.Top;
            Near = a_Other.Near;
            Far = a_Other.Far;
            return *this;
        }

    public:
        float Left, Down;
        float Right, Top;
        float Near, Far;
};

class Vertex3D {
    public:
        Vertex3D(glm::vec3 a_Position, glm::vec2 a_TexCoord, glm::vec3 a_Normal) {
            Position = a_Position;
            TexCoord = a_TexCoord;
            Normal = a_Normal;
        }
        Vertex3D(float a_PositionX, float a_PositionY, float a_PositionZ, float a_TexCoordX, float a_TexCoordY, float a_NormalX, float a_NormalY, float a_NormalZ) {
            Position = glm::vec3(a_PositionX, a_PositionY, a_PositionZ);
            TexCoord = glm::vec2(a_TexCoordX, a_TexCoordY);
            Normal = glm::vec3(a_NormalX, a_NormalY, a_NormalZ);
        }
        ~Vertex3D() {

        }

    public:
        glm::vec3 Position;
        glm::vec2 TexCoord;
        glm::vec3 Normal;
};
class VertexBuffer {
    public:
        virtual ~VertexBuffer() {

        }

        virtual void BufferData(const std::vector<Vertex3D>& a_Data) {

        }
        virtual void BufferData(Vertex3D* a_Data, unsigned int a_Count) {

        }

        virtual void Bind() const {

        }

        virtual unsigned int GetCount() const {

        }
};
class OpenGLVertexBuffer: public VertexBuffer {
    public:
        OpenGLVertexBuffer() {
            glCreateVertexArrays(1, &m_VARendererID);
            glBindVertexArray(m_VARendererID);

            glCreateBuffers(1, &m_VBRendererID);
            glBindBuffer(GL_ARRAY_BUFFER, m_VBRendererID);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), reinterpret_cast<void*>(0));
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), reinterpret_cast<void*>(sizeof(glm::vec3)));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), reinterpret_cast<void*>(sizeof(glm::vec3) + sizeof(glm::vec2)));
            glEnableVertexAttribArray(2);

            m_Count = 0;

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL VertexBuffer: " << m_VBRendererID << " : " << m_VARendererID << " Constructed! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME
        }
        OpenGLVertexBuffer(const std::vector<Vertex3D>& a_Data) {
            glCreateVertexArrays(1, &m_VARendererID);
            glBindVertexArray(m_VARendererID);

            glCreateBuffers(1, &m_VBRendererID);
            glBindBuffer(GL_ARRAY_BUFFER, m_VBRendererID);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), reinterpret_cast<void*>(0));
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), reinterpret_cast<void*>(sizeof(glm::vec3)));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), reinterpret_cast<void*>(sizeof(glm::vec3) + sizeof(glm::vec2)));
            glEnableVertexAttribArray(2);

            BufferData(a_Data);

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL VertexBuffer: " << m_VBRendererID << " : " << m_VARendererID << " Constructed! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME
        }
        OpenGLVertexBuffer(Vertex3D* a_Data, unsigned int a_Count) {
            glCreateVertexArrays(1, &m_VARendererID);
            glBindVertexArray(m_VARendererID);

            glCreateBuffers(1, &m_VBRendererID);
            glBindBuffer(GL_ARRAY_BUFFER, m_VBRendererID);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), reinterpret_cast<void*>(0));
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), reinterpret_cast<void*>(sizeof(glm::vec3)));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), reinterpret_cast<void*>(sizeof(glm::vec3) + sizeof(glm::vec2)));
            glEnableVertexAttribArray(2);

            BufferData(a_Data, a_Count);

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL VertexBuffer: " << m_VBRendererID << " : " << m_VARendererID << " Constructed! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME
        }
        ~OpenGLVertexBuffer() {
            glDeleteBuffers(1, &m_VBRendererID);
            glDeleteVertexArrays(1, &m_VARendererID);

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL VertexBuffer: " << m_VBRendererID << " : " << m_VARendererID << " Destructed! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME
        }

        void BufferData(const std::vector<Vertex3D>& a_Data) {
            Bind();
            m_Count = a_Data.size();
            glBufferData(GL_ARRAY_BUFFER, a_Data.size() * sizeof(Vertex3D), &a_Data[0], GL_STATIC_DRAW);

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL VertexBuffer: " << m_VBRendererID << " : " << m_VARendererID << " Send Data! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME
        }
        void BufferData(Vertex3D* a_Data, unsigned int a_Count) {
            Bind();
            m_Count = a_Count;
            glBufferData(GL_ARRAY_BUFFER, a_Count * sizeof(Vertex3D), a_Data, GL_STATIC_DRAW);

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL VertexBuffer: " << m_VBRendererID << " : " << m_VARendererID << " Send Data! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME
        }

        void Bind() const {
            glBindVertexArray(m_VARendererID);
            glBindBuffer(GL_ARRAY_BUFFER, m_VBRendererID);

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL VertexBuffer: " << m_VBRendererID << " : " << m_VARendererID << " Bound! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME
        }
        unsigned int GetCount() const {
            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL VertexBuffer: " << m_VBRendererID << " : " << m_VARendererID << " Count Given! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME

            return m_Count;
        }

        unsigned int GetVertexBufferRendererID() const {
            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL VertexBuffer: " << m_VBRendererID << " : " << m_VARendererID << " VertexBuffer RendererID Given! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME

            return m_VBRendererID;
        }
        unsigned int GetVertexArrayRendererID() const {
            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL VertexBuffer: " << m_VBRendererID << " : " << m_VARendererID << " VertexArray RendererID Given! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME

            return m_VARendererID;
        }

    private:
        unsigned int m_VBRendererID;
        unsigned int m_Count;
        unsigned int m_VARendererID;
};
std::vector<Vertex3D> LoadVerticesFromOBJFile(const std::string& a_Filename) {
    std::fstream f_File(a_Filename);

    if(!f_File.is_open()) {
        std::cerr <<  "Unable to find file: '" << a_Filename << "'!" << std::endl;
        return {};
    }

    std::vector<glm::vec3> f_Positions;
    std::vector<glm::vec2> f_TexCoords;
    std::vector<glm::vec3> f_Normals;

    std::vector<Vertex3D> f_FinalData;

    std::string f_Line;
    while(std::getline(f_File, f_Line)) {
        unsigned int f_SlashCount = 0;
        for(char& c : f_Line) {
            if(c == '/') {
                c = ' ';
                f_SlashCount++;
            }
        }

        unsigned int f_VerticesInFace = f_SlashCount / 2;

        std::strstream f_LineStream;
        f_LineStream << f_Line;

        if(f_Line[0] == 'v' && f_Line[1] == ' ') {
            std::string f_Junk;
            glm::vec3 f_Data;
            f_LineStream >> f_Junk >> f_Data.x >> f_Data.y >> f_Data.z;
            f_Positions.push_back(f_Data);
        }
        else if(f_Line[0] == 'v' && f_Line[1] == 't') {
            std::string f_Junk;
            glm::vec2 f_Data;
            f_LineStream >> f_Junk >> f_Data.x >> f_Data.y;
            f_TexCoords.push_back(f_Data);
        }
        else if(f_Line[0] == 'v' && f_Line[1] == 'n') {
            std::string f_Junk;
            glm::vec3 f_Data;
            f_LineStream >> f_Junk >> f_Data.x >> f_Data.y >> f_Data.z;
            f_Normals.push_back(f_Data);
        }
        else if(f_Line[0] == 'f' && f_Line[1] == ' ') {
            std::string f_Junk;

            if(f_VerticesInFace == 3) {
                unsigned int f_IndexPosition1, f_IndexTexCoord1, f_IndexNormal1,
                            f_IndexPosition2, f_IndexTexCoord2, f_IndexNormal2,
                            f_IndexPosition3, f_IndexTexCoord3, f_IndexNormal3;

                f_LineStream >> f_Junk >> f_IndexPosition1 >> f_IndexTexCoord1 >> f_IndexNormal1
                            >> f_IndexPosition2 >> f_IndexTexCoord2 >> f_IndexNormal2
                            >> f_IndexPosition3 >> f_IndexTexCoord3 >> f_IndexNormal3;

                f_FinalData.push_back(Vertex3D(f_Positions[f_IndexPosition1 - 1], f_TexCoords[f_IndexTexCoord1 - 1], f_Normals[f_IndexNormal1 - 1]));
                f_FinalData.push_back(Vertex3D(f_Positions[f_IndexPosition2 - 1], f_TexCoords[f_IndexTexCoord2 - 1], f_Normals[f_IndexNormal2 - 1]));
                f_FinalData.push_back(Vertex3D(f_Positions[f_IndexPosition3 - 1], f_TexCoords[f_IndexTexCoord3 - 1], f_Normals[f_IndexNormal3 - 1]));
            }
            else if(f_VerticesInFace == 4) {
                unsigned int f_IndexPosition1, f_IndexTexCoord1, f_IndexNormal1,
                            f_IndexPosition2, f_IndexTexCoord2, f_IndexNormal2,
                            f_IndexPosition3, f_IndexTexCoord3, f_IndexNormal3,
                            f_IndexPosition4, f_IndexTexCoord4, f_IndexNormal4;

                f_LineStream >> f_Junk >> f_IndexPosition1 >> f_IndexTexCoord1 >> f_IndexNormal1
                            >> f_IndexPosition2 >> f_IndexTexCoord2 >> f_IndexNormal2
                            >> f_IndexPosition3 >> f_IndexTexCoord3 >> f_IndexNormal3
                            >> f_IndexPosition4 >> f_IndexTexCoord4 >> f_IndexNormal4;

                f_FinalData.push_back(Vertex3D(f_Positions[f_IndexPosition1 - 1], f_TexCoords[f_IndexTexCoord1 - 1], f_Normals[f_IndexNormal1 - 1]));
                f_FinalData.push_back(Vertex3D(f_Positions[f_IndexPosition2 - 1], f_TexCoords[f_IndexTexCoord2 - 1], f_Normals[f_IndexNormal2 - 1]));
                f_FinalData.push_back(Vertex3D(f_Positions[f_IndexPosition3 - 1], f_TexCoords[f_IndexTexCoord3 - 1], f_Normals[f_IndexNormal3 - 1]));

                f_FinalData.push_back(Vertex3D(f_Positions[f_IndexPosition3 - 1], f_TexCoords[f_IndexTexCoord3 - 1], f_Normals[f_IndexNormal3 - 1]));
                f_FinalData.push_back(Vertex3D(f_Positions[f_IndexPosition4 - 1], f_TexCoords[f_IndexTexCoord4 - 1], f_Normals[f_IndexNormal4 - 1]));
                f_FinalData.push_back(Vertex3D(f_Positions[f_IndexPosition1 - 1], f_TexCoords[f_IndexTexCoord1 - 1], f_Normals[f_IndexNormal1 - 1]));
            }
            else {
                std::cerr << "Unable to load models with faces containing more than 4 vertices! Problematic file: " << a_Filename << std::endl;
                assert(false);
            }
        }
    }

    return f_FinalData;
}

class ShaderElement {
    public:
        virtual ~ShaderElement() {

        }

        virtual void Compile(unsigned int a_Type, const std::string& a_Source) {

        }
};
class OpenGLShaderElement: public ShaderElement {
    public:
        OpenGLShaderElement() {
            m_Created = false;
        }
        OpenGLShaderElement(unsigned int a_Type, const std::string& a_Source) {
            Compile(a_Type, a_Source);
        }
        ~OpenGLShaderElement() {
            if(m_Created) {
                glDeleteShader(m_RendererID);

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL ShaderElement: " << m_RendererID << " Destructed! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME
            }
        }

        void Compile(unsigned int a_Type, const std::string& a_Source) {
            m_Created = true;
            m_RendererID = glCreateShader(a_Type);

            const char* f_Source = a_Source.c_str();
            glShaderSource(m_RendererID, 1, &f_Source, nullptr);

            glCompileShader(m_RendererID);

            int f_Compiled;
            glGetShaderiv(m_RendererID, GL_COMPILE_STATUS, &f_Compiled);
            if(f_Compiled == GL_FALSE) {
                int f_Length;
                glGetShaderiv(m_RendererID, GL_INFO_LOG_LENGTH, &f_Length);

                std::string f_Message(f_Length, '\0');
                glGetShaderInfoLog(m_RendererID, f_Length, &f_Length, &f_Message[0]);

                std::cerr << "[SHADER COMPILATION ERROR]" << f_Message << std::endl;
            }

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL ShaderElement: " << m_RendererID << " Constructed! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME
        }

        unsigned int GetRendererID() const {
            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL ShaderElement: " << m_RendererID << " Renderer ID Given! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME

            return m_RendererID;
        }

    private:
        unsigned int m_RendererID;
        bool m_Created;
};
class Shader {
    public:
        virtual ~Shader() {

        }

        virtual void AttachShaderElement(const ShaderElement* a_ShaderElement) {

        }

        virtual void Compile() {

        }

        virtual void Bind() const {

        }
};
class OpenGLShader: public Shader {
    public:
        OpenGLShader() {
            m_RendererID = glCreateProgram();

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL Shader: " << m_RendererID << " Constructed! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME
        }
        OpenGLShader(const ShaderElement* a_VertexShader, const ShaderElement* a_FragmentShader, bool a_Compile, bool a_DeleteShaderElements) {
            m_RendererID = glCreateProgram();

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL Shader: " << m_RendererID << " Constructed! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME

            AttachShaderElement(a_VertexShader);
            AttachShaderElement(a_FragmentShader);

            if(a_Compile) {
                Compile();
            }
            if(a_DeleteShaderElements) {
                delete a_VertexShader;
                delete a_FragmentShader;
            }
        }
        ~OpenGLShader() {
            glDeleteProgram(m_RendererID);
        }

        void AttachShaderElement(const ShaderElement* a_ShaderElement) {
            glAttachShader(m_RendererID, dynamic_cast<const OpenGLShaderElement*>(a_ShaderElement)->GetRendererID());

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL Shader: " << m_RendererID << " ShaderElement Attached: " << dynamic_cast<const OpenGLShaderElement*>(a_ShaderElement)->GetRendererID() << " !" << std::endl;
            #endif // VENUM_DEBUG_EXTREME
        }

        void Compile() {
            glLinkProgram(m_RendererID);
            glValidateProgram(m_RendererID);

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL Shader: " << m_RendererID << " Compiled! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME

            Bind();
        }

        void Bind() const {
            glUseProgram(m_RendererID);

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL Shader: " << m_RendererID << " Bound! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME
        }

        unsigned int GetRendererID() const {
            return m_RendererID;
        }

        void SetUniform(const std::string& Name, glm::mat4 Value) {
            Bind();
            int location = glGetUniformLocation(m_RendererID, Name.c_str());
            assert(location != -1);
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(Value));
        }
        void SetUniform(const std::string& Name, glm::vec4 Value) {
            Bind();
            int location = glGetUniformLocation(m_RendererID, Name.c_str());
            assert(location != -1);
            glUniform4fv(location, 1, glm::value_ptr(Value));
        }
        void SetUniform(const std::string& Name, glm::vec3 Value) {
            Bind();
            int location = glGetUniformLocation(m_RendererID, Name.c_str());
            assert(location != -1);
            glUniform3fv(location, 1, glm::value_ptr(Value));
        }
        void SetUniform(const std::string& Name, glm::vec2 Value) {
            Bind();
            int location = glGetUniformLocation(m_RendererID, Name.c_str());
            assert(location != -1);
            glUniform2fv(location, 1, glm::value_ptr(Value));
        }
        void SetUniform(const std::string& Name, float Value) {
            Bind();
            int location = glGetUniformLocation(m_RendererID, Name.c_str());
            assert(location != -1);
            glUniform1f(location, Value);
        }
        void SetUniform(const std::string& Name, int Value) {
            Bind();
            int location = glGetUniformLocation(m_RendererID, Name.c_str());
            assert(location != -1);
            glUniform1i(location, Value);
        }

    private:
        unsigned int m_RendererID;
};

class FrameBuffer {
    public:
        virtual ~FrameBuffer() {

        }

        virtual void Bind() const {

        }

    public:
        unsigned int Width, Height;
};
class OpenGLFrameBuffer: public FrameBuffer {
    public:
        OpenGLFrameBuffer(unsigned int a_Width, unsigned int a_Height) {
            glCreateFramebuffers(1, &m_RendererID);
            Bind();

            Width = a_Width;
            Height = a_Height;
        }
        ~OpenGLFrameBuffer() {
            glDeleteFramebuffers(1, &m_RendererID);
        }

        void Bind() const {
            glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        }

        unsigned int GetRendererID() const {
            return m_RendererID;
        }

    private:
        unsigned int m_RendererID;
};

class TextureSettings {
    public:
        TextureSettings(unsigned int a_Interpolation = GL_LINEAR, unsigned int a_Wrapping = GL_CLAMP_TO_EDGE) {
            Interpolation = a_Interpolation;
            Wrapping = a_Wrapping;
        }
        ~TextureSettings() {

        }

    public:
        unsigned int Interpolation;
        unsigned int Wrapping;
};
class Texture2D {
    public:
        virtual ~Texture2D() {

        }

        virtual void BufferData(const std::string& a_Filename, const TextureSettings& a_Settings = TextureSettings()) {

        }
        virtual void BufferData(FrameBuffer* a_FrameBuffer, const TextureSettings& a_Settings = TextureSettings(), bool a_DepthStencil = false, unsigned int a_ColorAttachment = 0) {

        }

        virtual void Bind(unsigned int index = 0) const {

        }

};
class OpenGLTexture2D: public Texture2D {
    public:
        OpenGLTexture2D() {
            glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        }
        OpenGLTexture2D(const std::string& a_Filename, const TextureSettings& a_Settings) {
            glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
            BufferData(a_Filename, a_Settings);
        }
        ~OpenGLTexture2D() {
            glDeleteTextures(1, &m_RendererID);
        }

        void BufferData(const std::string& a_Filename, const TextureSettings& a_Settings) {
            SDL_Surface* f_Surface = IMG_Load(a_Filename.c_str());

            glBindTexture(GL_TEXTURE_2D, m_RendererID);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, f_Surface->w, f_Surface->h, 0, (f_Surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, f_Surface->pixels);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, a_Settings.Interpolation);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, a_Settings.Interpolation);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, a_Settings.Wrapping);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, a_Settings.Wrapping);

            SDL_FreeSurface(f_Surface);
        }
        void BufferData(FrameBuffer* a_FrameBuffer, const TextureSettings& a_Settings, bool a_DepthStencil, unsigned int a_ColorAttachment) {
            a_FrameBuffer->Bind();
            glBindTexture(GL_TEXTURE_2D, m_RendererID);

            glTexImage2D(GL_TEXTURE_2D, 0, (a_DepthStencil ? GL_DEPTH24_STENCIL8 : GL_RGBA8), a_FrameBuffer->Width, a_FrameBuffer->Height, 0, (a_DepthStencil ? GL_DEPTH_STENCIL : GL_RGBA), (a_DepthStencil ? GL_UNSIGNED_INT_24_8 : GL_UNSIGNED_BYTE), nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, a_Settings.Interpolation);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, a_Settings.Interpolation);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, a_Settings.Wrapping);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, a_Settings.Wrapping);

            glFramebufferTexture2D(GL_FRAMEBUFFER, (a_DepthStencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_COLOR_ATTACHMENT0 + a_ColorAttachment), GL_TEXTURE_2D, m_RendererID, 0);
        }

        void Bind(unsigned int index = 0) const {
            assert(index < 31);
            glActiveTexture(GL_TEXTURE0 + index);
            glBindTexture(GL_TEXTURE_2D, m_RendererID);
        }

        unsigned int GetRendererID() {
            return m_RendererID;
        }

    private:
        unsigned int m_RendererID;
};

class Viewport {
    public:
        Viewport(glm::ivec2 a_Position = glm::vec2(0.0f, 0.0f), glm::uvec2 a_Size = glm::vec2(640.0f, 360.0f)) {
            Position = a_Position;
            Size = a_Size;

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "Viewport Constructed! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME
        }
        ~Viewport() {

        }

        float GetAspect() const {
            return static_cast<float>(Size.x) / static_cast<float>(Size.y);

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "'GetAspect()' Used! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME
        }

    public:
        glm::ivec2 Position;
        glm::uvec2 Size;
};

enum class API {
    None = 0,
    OpenGL = 1,
};
class Renderer {
    public:
        virtual ~Renderer() {

        }

        virtual VertexBuffer* VertexBufferCreate() {
            return nullptr;
        }
        virtual VertexBuffer* VertexBufferCreate(const std::vector<Vertex3D>& a_Data) {
            return nullptr;
        }
        virtual VertexBuffer* VertexBufferCreate(Vertex3D* a_Data, unsigned int a_Count) {
            return nullptr;
        }

        virtual ShaderElement* ShaderElementCreate() {
            return nullptr;
        }
        virtual ShaderElement* ShaderElementCreate(unsigned int a_Type, const std::string& a_Source) {
            return nullptr;
        }

        virtual Shader* ShaderCreate() {
            return nullptr;
        }
        virtual Shader* ShaderCreate(const ShaderElement* a_VertexShader, const ShaderElement* a_FragmentShader, bool a_Compile = true, bool a_DeleteShaderElements = true) {
            return nullptr;
        }

        virtual FrameBuffer* FrameBufferCreate(unsigned int a_Width = 0, unsigned int a_Height = 0) {
            return nullptr;
        }

        virtual Texture2D* Texture2DCreate() {
            return nullptr;
        }
        virtual Texture2D* Texture2DCreate(const std::string& a_Filename, const TextureSettings& a_Settings = TextureSettings()) {
            return nullptr;
        }

        virtual void Init() {

        }
        virtual void Clear(bool a_ClearColor = true, bool a_ClearDepth = true, bool a_ClearStencil = false) {

        }
        virtual void SetViewport(const Viewport& a_Viewport = Viewport()) {

        }
        virtual void Draw(VertexBuffer* a_VertexBuffer, Shader* a_Shader) {

        }

        virtual API GetAPI() const {
            return API::None;
        }
        virtual std::string GetAPIString() const {
            return "None";
        }
};
class OpenGLRenderer: public Renderer {
    public:
        OpenGLRenderer() {

        }
        ~OpenGLRenderer() {

        }

        VertexBuffer* VertexBufferCreate() {
            return new OpenGLVertexBuffer();
        }
        VertexBuffer* VertexBufferCreate(const std::vector<Vertex3D>& a_Data) {
            return new OpenGLVertexBuffer(a_Data);
        }
        VertexBuffer* VertexBufferCreate(Vertex3D* a_Data, unsigned int a_Count) {
            return new OpenGLVertexBuffer(a_Data, a_Count);
        }

        ShaderElement* ShaderElementCreate() {
            return new OpenGLShaderElement();
        }
        ShaderElement* ShaderElementCreate(unsigned int a_Type, const std::string& a_Source) {
            return new OpenGLShaderElement(a_Type, a_Source);
        }

        Shader* ShaderCreate() {
            return new OpenGLShader();
        }
        Shader* ShaderCreate(const ShaderElement* a_VertexShader, const ShaderElement* a_FragmentShader, bool a_Compile = true, bool a_DeleteShaderElements = true) {
            return new OpenGLShader(a_VertexShader, a_FragmentShader, a_Compile, a_DeleteShaderElements);
        }

        FrameBuffer* FrameBufferCreate(unsigned int a_Width = 0, unsigned int a_Height = 0) {
            return new OpenGLFrameBuffer(a_Width, a_Height);
        }

        Texture2D* Texture2DCreate() {
            return new OpenGLTexture2D();
        }
        Texture2D* Texture2DCreate(const std::string& a_Filename, const TextureSettings& a_Settings = TextureSettings()) {
            return new OpenGLTexture2D(a_Filename, a_Settings);
        }

        void Init() {
            glEnable(GL_DEPTH_TEST);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL Renderer: " << "Initialized! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME
        }
        void Clear(bool a_ClearColor = true, bool a_ClearDepth = true, bool a_ClearStencil = false) {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear((a_ClearColor ? GL_COLOR_BUFFER_BIT : 0) | (a_ClearDepth ? GL_DEPTH_BUFFER_BIT : 0) | (a_ClearStencil ? GL_STENCIL_BUFFER_BIT : 0));

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL Renderer: " << "Cleared! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME
        }
        void SetViewport(const Viewport& a_Viewport = Viewport()) {
            glViewport(a_Viewport.Position.x, a_Viewport.Position.y, a_Viewport.Size.x, a_Viewport.Size.y);
        }
        void Draw(VertexBuffer* a_VertexBuffer, Shader* a_Shader) {
            a_Shader->Bind();
            a_VertexBuffer->Bind();

            glDrawArrays(GL_TRIANGLES, 0, a_VertexBuffer->GetCount());

            #ifdef VENUM_DEBUG_EXTREME
            std::cout << "OpenGL Renderer: " << "Drawn! " << std::endl;
            #endif // VENUM_DEBUG_EXTREME
        }

        API GetAPI() const {
            return API::OpenGL;
        }
        std::string GetAPIString() const {
            return "OpenGL";
        }
};

template<typename T> class ScopePointer {
    public:
        explicit ScopePointer(T* a_Pointer = nullptr) {
            m_Pointer = a_Pointer;
        }
        ScopePointer(const ScopePointer<T>& a_Copy) = delete;
        ~ScopePointer() {
            delete m_Pointer;
        }

        ScopePointer<T>& operator=(T* a_Pointer) = delete;
        ScopePointer<T>& operator=(const ScopePointer<T>& a_Copy) = delete;

        ScopePointer<T>& operator()(T* a_Pointer) {
            if(a_Pointer != m_Pointer) {
                delete m_Pointer;
                m_Pointer = a_Pointer;
            }
            return *this;
        }

        operator T*() {
            return m_Pointer;
        }
        T* operator->() {
            return m_Pointer;
        }
        T& operator*() {
            return *m_Pointer;
        }

        T* Get() {
            return m_Pointer;
        }

        T* Loose() {
            T* f_Returned = m_Pointer;
            m_Pointer = nullptr;
            return f_Returned;
        }

        template<typename Y> Y* DynamicCast() {
            return dynamic_cast<Y*>(m_Pointer);
        }

    private:
        T* m_Pointer;
};

/* Venum::Main() is the function in which your main code will run. */
int Main();

}

/* Definition of main, which initializes SDL, runs the Venum::Main() and quits SDL. */
int main(int argc, char** argv) {
    /* Initializing SDL */
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP);
    #ifdef Mixer
    Mix_Init(MIX_INIT_FLAC | MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_MID | MIX_INIT_MOD | MIX_INIT_OPUS);
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        return false;
    }
    #endif // Mixer

    /* Running the Venum::Main() function. */
    int f_Returned = Venum::Main();

    /* Quitting SDL */
    #ifdef Mixer
    Mix_Quit();
    #endif // Mixer
    IMG_Quit();
    SDL_Quit();

    return f_Returned;
}

#endif // ENGINE_HPP_INCLUDED
