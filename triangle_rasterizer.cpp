// Simple Direct Media Layer 3 (SDL3): https://wiki.libsdl.org/SDL3/FrontPage
// SDL3 download: https://github.com/libsdl-org/SDL/releases/tag/release-3.2.26
// Claude AI: Used for debugging, guidance, emotional support
/*
    Simple DirectMedia Layer is a cross-platform development library designed 
    to provide low level access to audio, keyboard, mouse, joystick, 
    and graphics hardware via OpenGL/Direct3D/Metal/Vulkan. 
    It is used by video playback software, emulators, 
    and popular games including Valve's award winning catalog and many Humble Bundle games.
*/

/*
    Bresenham's Line algorithm is used to draw edges between the vertices on a 2D plane.
    Its implementation is complex and beyond the scope of this project. For that reason,
    I've copied an implementation from Wikipedia and modified it to work with SDL3: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
*/

/*
    Color hex codes: https://html-color.codes/
*/

/*
    How to Rasterize a Triangle: https://www.youtube.com/watch?v=t7Ztio8cwqM
*/

#include <iostream>
#include <string>
#include <SDL3/SDL.h>
#include <vector>
using namespace std;

int SCREEN_WIDTH = 500;
int SCREEN_HEIGHT = 500;

// All colors have full alpha
Uint32 RED = 0xFF0000FF;
Uint32 GREEN = 0x00FF00FF;
Uint32 BLUE = 0x0000FFFF;  
Uint32 ORANGE = 0xFFA500FF;
Uint32 GOLD = 0xFFD700FF;
Uint32 PINK = 0xFFC0CBFF;



struct Screen {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    int width;
    int height;
    Uint32* pixels; // the pixel buffer (1D array), stores RGBA color (format: 0xRRGGBBAA)
};

struct Vertex {
    int x;
    int y;
    Uint32 color;
};

// Draws the screen where the triangles will be rendered
Screen drawScreen(int width, int height) {
    Screen screen;
    // NOTE: In computer graphics, the screen's origin is in the top-left pixel at (0, 0)
    /*
          0 1 2 3 4 5
        0
        1
        2
        3
        4
        5
    
    */
    // Tell SDL to initialize its video system
    // This allows us to create windows and render graphics
    // Error checking (optional but good practice)
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        cout << "SDL_INIT failed: " << SDL_GetError() << endl;
        return screen;
    }

    // Create the window
    SDL_Window* window = SDL_CreateWindow(
        "Triangle Rasterizer",  // window title
        width,                  // width in pixels
        height,                 // height in pixels
        0                       // flags (0 = default window)
    );

    // Check for window errors
    if (!window) {
        cout << "Window creation failed: " << SDL_GetError() << endl;
    }

    // Create the renderer
    // The renderer is what actually draws things to the window
    /*
        @window: the window to render to
        @NULL: let SDL pick the best rendering driver (can also be a specific driver name)
    */
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    // Create the texture (the pixel buffer)
    // The texture is the actual pixel array that SDL displays
    /*
        @renderer: the renderer that will use this texture
        @ SDL_PIXELFORMAT_RGBA8888: each pixel is 32 bits: red, green, blue, alpha (8 bits each)
                                    each pixels is a Uint32
                                    red in highest byte, alpha in lowest
        @SDL_TEXTUREACCESS_STREAMING: allows us to update the texture every frame
        @width, height: size of texture in pixels
    */
    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,    // pixel format
        SDL_TEXTUREACCESS_STREAMING, // access pattern
        width,
        height
    );

    /*
        Allocate the pixel array
        Why 1D array?
            - easier to manage memory
            - faster access
            - SDL texture uses 1D arrays internally
    */
    Uint32* pixels = new Uint32[width * height];

    /*
        How to access pixel at (x, y)
        
        int index = y * width + x;
        pixels[index] = 0xFF0000FF; red pixel

    */

    // Initialize the pixels to black
    for (int i = 0; i < width * height; i++) {
        pixels[i] = 0x000000FF; // Black with full alpha
    }

    // Put it all in a screen struct and return
    screen.window = window;
    screen.renderer = renderer;
    screen.texture = texture;
    screen.width = width;
    screen.height = height;
    screen.pixels = pixels;

    return screen;
}


void updateScreen(Screen& screen) {
    // Step 1: Update the texture with pixel data
    SDL_UpdateTexture(
        screen.texture,                 // the texture to update
        NULL,                           // update entire texture (NULL = whole thing)
        screen.pixels,                  // the pixel array
        screen.width * sizeof(Uint32)   // pitch (bytes per row)
    );

    // Step 2: Copy texture to renderer
    /*
        @first NULL: source rectangle (NULL = entire texture)
        @second NULL: destination rectangle (NULL = entire window)
    */
    SDL_RenderTexture(screen.renderer, screen.texture, NULL, NULL);

    // Step 3: Present (show) what is rendered
    SDL_RenderPresent(screen.renderer);
}

// Set a single pixel (with bounds checking)
void setPixel(Screen& screen, int x, int y, Uint32 color) {
    if (x < 0 || x >= screen.width || y < 0 || y >= screen.height) {
        return;
    }
    int index = y * screen.width + x;
    screen.pixels[index] = color;
}

/*
    Helper function that interpolates between two colors
    Used in bresenham functions
*/
Uint32 interpolateColor(Uint32 color0, Uint32 color1, float t) {
    // Extract R, G, B, A from color0
    uint8_t r1 = (color0 >> 24) & 0xFF;
    uint8_t g1 = (color0 >> 16) & 0xFF;
    uint8_t b1 = (color0 >> 8) & 0xFF;
    uint8_t a1 = color0 & 0xFF;

    // Extract R, G, B, A from color1
    uint8_t r2 = (color1 >> 24) & 0xFF;
    uint8_t g2 = (color1 >> 16) & 0xFF;
    uint8_t b2 = (color1 >> 8) & 0xFF;
    uint8_t a2 = color1 & 0xFF;

    // Interpolate each component
    uint8_t r = (uint8_t)(r1 + (r2 - r1) * t);
    uint8_t g = (uint8_t)(g1 + (g2 - g1) * t);  
    uint8_t b = (uint8_t)(b1 + (b2 - b1) * t);  
    uint8_t a = (uint8_t)(a1 + (a2 - a1) * t);

    // Pack into new color
    Uint32 result = (r << 24) | (g << 16) | (b << 8) | a;
    
    return result;
}

/*
    Bresenham's Line Algorithm - RETURNS pixels instead of drawing them
    Based on Wikipedia: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
*/

// Helper for shallow lines - returns list of pixels
vector<Vertex> bresenhamLow(int x0, int y0, Uint32 color0, 
                            int x1, int y1, Uint32 color1) {
    vector<Vertex> pixels;
    int dx = x1 - x0;
    int dy = y1 - y0;
    int yi = 1;
    
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    
    int D = (2 * dy) - dx;
    int y = y0;

    int totalSteps = dx; // Total pixels along the line
    int currentStep = 0; // Which pixel we're on
    
    for (int x = x0; x <= x1; x++) {
        // Calculate t (0.0 at start, 1.0 at end)
        float t = (totalSteps > 0) ? (float)currentStep / (float)totalSteps : 0.0f;

        // Interpolate color for this pixel
        Uint32 color = interpolateColor(color0, color1, t);

        Vertex v = {x, y, color}; // Use interpolated color
        pixels.push_back(v);

        currentStep++; // Move to next pixel

        if (D > 0) {
            y = y + yi;
            D = D + (2 * (dy - dx));
        } else {
            D = D + 2 * dy;
        }
    }
    return pixels;
}

// Helper for steep lines - returns list of pixels
vector<Vertex> bresenhamHigh(int x0, int y0, Uint32 color0, 
                             int x1, int y1, Uint32 color1) {
    vector<Vertex> pixels;
    int dx = x1 - x0;
    int dy = y1 - y0;
    int xi = 1;
    
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }
    
    int D = (2 * dx) - dy;
    int x = x0;

    int totalSteps = dy; // Total pixels along the line
    int currentStep = 0; // Which pixel we're on
    
    for (int y = y0; y <= y1; y++) {
        // Calculate t (0.0 at start, 1.0 at end)
        float t = (totalSteps > 0) ? (float)currentStep / (float)totalSteps : 0.0f;

        // Interpolate color for this pixel
        Uint32 color = interpolateColor(color0, color1, t);
        
        Vertex v = {x, y, color};  // Use interpolated color
        pixels.push_back(v);

        currentStep++;
        
        if (D > 0) {
            x = x + xi;
            D = D + (2 * (dx - dy));
        } else {
            D = D + 2 * dx;
        }
    }
    return pixels;
}

// Main Bresenham - returns list of pixels on the line
vector<Vertex> bresenham(int x0, int y0, Uint32 color0, 
                         int x1, int y1, Uint32 color1) {
    if (abs(y1 - y0) < abs(x1 - x0)) {
        if (x0 > x1) {
            return bresenhamLow(x1, y1, color1, x0, y0, color0);
        } else {
            return bresenhamLow(x0, y0, color0, x1, y1, color1);
        }
    } else {
        if (y0 > y1) {
            return bresenhamHigh(x1, y1, color1, x0, y0, color0);
        } else {
            return bresenhamHigh(x0, y0, color0, x1, y1, color1);
        }
    }
}

// Draw triangle edges - collects pixels from all three edges
// This function is deprecated (replaced with fillTriangle())
void drawTriangle(Screen& screen, Vertex v0, Vertex v1, Vertex v2) {
    // Step 1: Collect pixels from all three edges
    vector<Vertex> edge1 = bresenham(v0.x, v0.y, v0.color, v1.x, v1.y, v1.color);
    vector<Vertex> edge2 = bresenham(v1.x, v1.y, v1.color, v2.x, v2.y, v2.color);
    vector<Vertex> edge3 = bresenham(v2.x, v2.y, v2.color, v0.x, v0.y, v0.color);
    
    // Step 2: Draw all edge pixels
    for (const Vertex& v : edge1) {
        setPixel(screen, v.x, v.y, v.color);
    }
    for (const Vertex& v : edge2) {
        setPixel(screen, v.x, v.y, v.color);
    }
    for (const Vertex& v : edge3) {
        setPixel(screen, v.x, v.y, v.color);
    }
}

void fillTriangle(Screen& screen, Vertex v0, Vertex v1, Vertex v2) {
    // Step 1: Sort vertices by Y coordinate (top to bottom)
    // We want v0.y <= v1.y <= v2.y
    if (v0.y > v1.y) swap(v0, v1);
    if (v0.y > v2.y) swap(v0, v2);
    if(v1.y > v2.y) swap(v1, v2);

    // Step 2: Handle degenerate case (flat line)
    if (v0.y == v2.y) return;

    // Step 3: Scan from top to bottom
    for (int y = v0.y; y <= v2.y; y++) {
        // Determine if we're in the top half or the bottom half of the triangle
        bool topHalf = y < v1.y;

        // Calculate the "short" edge slope based on which half
        int y_start, y_end;
        Vertex v_start, v_end;

        if (topHalf) {
            // Top half: from v0 to v1
            if (v1.y == v0.y) continue; // Skip if flat
            y_start = v0.y;
            y_end = v1.y;
            v_start = v0;
            v_end = v1;
        } else {
            // Bottom half: from v1 to v2
            if (v2.y == v1.y) continue; // Skip if flat
            y_start = v1.y;
            y_end = v2.y;
            v_start = v1;
            v_end = v2;
        }

        // Calculate x positions on both edges for this scanline
        float t_long = (float)(y - v0.y) / (float)(v2.y - v0.y);
        float x_long = v0.x + (v2.x - v0.x) * t_long;

        float t_short = (float)(y - y_start) / (float)(y_end - y_start);
        float x_short = v_start.x + (v_end.x - v_start.x) * t_short;

        // Calculate colors at both edge points
        Uint32 color_long = interpolateColor(v0.color, v2.color, t_long);
        Uint32 color_short = interpolateColor(v_start.color, v_end.color, t_short);

        // Make sure x_left is actually on the left
        int x_left = (int)min(x_long, x_short);
        int x_right = (int)max(x_long, x_short);
        Uint32 color_left = (x_long < x_short) ? color_long : color_short;
        Uint32 color_right = (x_long < x_short) ? color_short : color_long;

        // Fill horizontal span from left to right
        for (int x = x_left; x <= x_right; x++) {
            if (x_right == x_left) {
                setPixel(screen, x, y, color_left);
            } else {
                float t_span = (float)(x - x_left) / (float)(x_right - x_left);
                Uint32 color = interpolateColor(color_left, color_right, t_span);
                setPixel(screen, x, y, color);
            }
        }
    }
}

// Helper function to check if the three vertices are collinear (on the same line)
bool isCollinear(Vertex v0, Vertex v1, Vertex v2) {
    // Calculate the area using cross product
    // If area is 0, points are collinear
    int area = (v1.x - v0.x) * (v2.y - v0.y) - (v2.x - v0.x) * (v1.y - v0.y);
    return area == 0;
}


int main() {
    Screen screen = drawScreen(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Ask user if they want to add their own triangles, or use defaults
    int customTriangles = 0;
    cout << "Default Mode (1): Render default triangles\n";
    cout << "OR\n";
    cout << "Custom Mode (2): Draw your own!\n";
    cout << "If you fail to enter either (1) or (2), there will be consequences.\n";

    cin >> customTriangles;

    if (customTriangles != 1 && customTriangles != 2) {
        cout << "Your inability to follow basic instructions has caused this program to terminate itself.\n";
        cout << "You monster.\n\n";
        return 0;
    }

    if (customTriangles == 2) {
        // Ask user how many triangles they want
        int numTriangles;
        cout << "\n\nHow many triangles would you like to render? ";
        cin >> numTriangles;

        cout << "\n\nNOTE: The window has valid coordinates between (0, 0) and (499, 499).\n";
        cout << "(0, 0) is the top-left most pixel.\n";
        cout << "You can draw vertices outside of these bounds, see what happens!\n\n";

        // Store all triangles
        vector<vector<Vertex>> triangles;

        // Get input for each triangle
        for (int i = 0; i < numTriangles; i++) {
            cout << "\n=== Triangle " << (i + 1) << " ===\n";
            vector<Vertex> triangle;

            // Get 3 vertices for this triangle
            for (int v = 0; v < 3; v++) {
                Vertex vertex;
                cout << "Vertex " << (v + 1) << ":\n";

                cout << " x: ";
                cin >> vertex.x;

                cout << " y: ";
                cin >> vertex.y;

                // Ask for color
                cout << " color (1=Red, 2=Green, 3=Blue, 4=Orange, 5=Gold, 6=Pink): ";
                int colorChoice;
                cin >> colorChoice;

                if (colorChoice == 1) vertex.color = RED;
                else if (colorChoice == 2) vertex.color = GREEN;
                else if (colorChoice == 3) vertex.color = BLUE;
                else if (colorChoice == 4) vertex.color = ORANGE;
                else if (colorChoice == 5) vertex.color = GOLD;
                else if (colorChoice == 6) vertex.color = PINK;
                else vertex.color = 0xFFFFFFFF; // White if invalid

                triangle.push_back(vertex);
            }

            // Check if vertices are collinear
            if (isCollinear(triangle[0], triangle[1], triangle[2])) {
                cout << "ERROR: Vertices are collinear (form a line, not a triangle)\n";
                cout << "Triangle " << (i+1) << " is invalid. Please try again.\n";
                i--; // Retry this triangle 
            } else {
                triangles.push_back(triangle);
                cout << "Triangle " << (i+1) << " added successfully!" << endl;
            }
        }
        
        // Draw all triangles
        for (const auto& triangle: triangles) {
            fillTriangle(screen, triangle[0], triangle[1], triangle[2]);
        }

    } else {
        cout << "You have opted to render default triangles.\n";
        
        // Define two triangles
        Vertex v0 = {250, 100, RED};
        Vertex v1 = {100, 400, GREEN};
        Vertex v2 = {400, 400, BLUE};

        Vertex v3 = {100, 50, ORANGE};
        Vertex v4 = {50, 200, GOLD};    
        Vertex v5 = {200, 150, PINK};    
    

        fillTriangle(screen, v0, v1, v2);
        fillTriangle(screen, v3, v4, v5);
    }
    
    
    // Event loop
    bool running = true;
    SDL_Event event;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }
        updateScreen(screen);
        SDL_Delay(16);
    }
    
    // Cleanup
    delete[] screen.pixels;
    SDL_DestroyTexture(screen.texture);
    SDL_DestroyRenderer(screen.renderer);
    SDL_DestroyWindow(screen.window);
    SDL_Quit();
    
    return 0;
}