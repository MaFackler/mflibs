#define MF_RENDERER_IMPLEMENTATION
#include <mf_renderer.h>

namespace example_tetris {

static v3 BLACK{0.0f, 0.0f, 0.0f};
static v3 GRAY{0.2f, 0.2f, 0.2f};

static v3 BACKGROUND_LIGHT = mf::v3_create_255_to_1<f32>(0xfb, 0xf1, 0xc7);
static v3 BACKGROUND_GRAY = mf::v3_create_255_to_1<f32>(0x3c, 0x38, 0x36);


struct Piece
{
    u8 *data;
    u8 dim;

    u8 get(u8 col, u8 row, u8 direction)
    {
        u8 res = 0;
        if (direction == 0)
        {
            res = this->data[this->dim * row + col];
        }
        else if (direction == 1)
        {
            res = this->data[this->dim * (this->dim - 1 - col) + row];
        }
        else if (direction == 2)
        {
            res = this->data[this->dim * (this->dim - 1 - row) + this->dim - 1 - col];
        }
        else if (direction == 3)
        {
            res = this->data[this->dim * col + this->dim - 1 - row];
        }
        return res;
    }


    u32 getRowOffset(u8 direction)
    {
        u32 res = 0;
        for (size_t y = 0; y < this->dim; ++y)
        {
            for (size_t x = 0; x < this->dim; ++x)
            {
                u8 value = this->get(x, y, direction);
                if (value)
                    return res;

            }
            res++;
        }
        return res;
    }
};


u8 Piece1[] = {
    0, 0, 0, 0,
    1, 1, 1, 1,
    0, 0, 0, 0,
    0, 0, 0, 0,
};

u8 Piece2[] = {
    2, 2,
    2, 2,
};

u8 Piece3[] = {
    3, 3, 3,
    3, 0, 0,
    0, 0, 0,
};

u8 Piece4[] = {
    0, 4, 0,
    4, 4, 4,
    0, 0, 0,
};

u8 Piece5[] = {
    5, 5, 0,
    0, 5, 5,
    0, 0, 0,
};

u8 Piece6[] = {
    6, 6, 6,
    0, 0, 6,
    0, 0, 0,
};

u8 Piece7[] = {
    0, 7, 7,
    7, 7, 0,
    0, 0, 0,
};

static v3 colors[] = {
    mf::v3_create_255_to_1<f32>(0xfb, 0x49, 0x34), // red
    mf::v3_create_255_to_1<f32>(0xb8, 0xbb, 0x26), // green
    mf::v3_create_255_to_1<f32>(0x93, 0xa5, 0x98), // blue
    mf::v3_create_255_to_1<f32>(0xfa, 0xbd, 0x2f), // yellow
    mf::v3_create_255_to_1<f32>(0x8e, 0xc0, 0x7c), // aqua
    mf::v3_create_255_to_1<f32>(0xd3, 0x86, 0x9b), // purple
    mf::v3_create_255_to_1<f32>(0xeb, 0xdb, 0xb2), // gray/white
};

static v3 colorsLight[] = {
    mf::v3_create_255_to_1<f32>(0xcc, 0x24, 0x1d), // red
    mf::v3_create_255_to_1<f32>(0x98, 0x97, 0x1a), // green
    mf::v3_create_255_to_1<f32>(0x45, 0x85, 0x88), // blue
    mf::v3_create_255_to_1<f32>(0xd7, 0x99, 0x21), // yellow
    mf::v3_create_255_to_1<f32>(0x68, 0x9d, 0x6a), // aqua
    mf::v3_create_255_to_1<f32>(0xb1, 0x62, 0x86), // purple
    mf::v3_create_255_to_1<f32>(0xa8, 0x99, 0x84), // gray/white
};

static v3 colorsDark[] = {
    mf::v3_create_255_to_1<f32>(0xcc, 0x24, 0x1d), // red
    mf::v3_create_255_to_1<f32>(0x98, 0x97, 0x1a), // green
    mf::v3_create_255_to_1<f32>(0x45, 0x85, 0x88), // blue
    mf::v3_create_255_to_1<f32>(0xd7, 0x99, 0x21), // yellow
    mf::v3_create_255_to_1<f32>(0x68, 0x9d, 0x6a), // aqua
    mf::v3_create_255_to_1<f32>(0xb1, 0x62, 0x86), // purple
    mf::v3_create_255_to_1<f32>(0xa8, 0x99, 0x84), // gray/white
};

static Piece piecesDef[] = {
    Piece{Piece1, 4},
    Piece{Piece2, 2},
    Piece{Piece3, 3},
    Piece{Piece4, 3},
    Piece{Piece5, 3},
    Piece{Piece6, 3},
    Piece{Piece7, 3},
};

enum GamePhase
{
    GAME_RUNNING,
    GAME_OVER,
};

struct GameState
{
    u32 numBlocksX = 10;
    u32 numBlocksY = 22;
    u32 blockSize = 40;
    GamePhase phase = GAME_RUNNING;
    u8 *board = {};
    float deltaSec = 0.0f;
    i32 row = 0;
    i32 col = 0;
    u8 direction = 0;
    float tickTime = 0.5f;
    u32 currentPieceIndex = 0;
    Piece *pieces;
    u32 numPieces = 0;

    u32 linesDeleted = 0;

    Piece* getCurrentPiece() { return &this->pieces[this->currentPieceIndex]; }
    void generateNewPiece();
    void setStartRowOfPiece();

};



inline
u32 GetRandomNumber(u32 min, u32 max)
{
    static bool initialized = false;
    if (!initialized)
    {
        srand(time(NULL));
        initialized = true;
    }
    u32 res = rand() % (max + 1 - min) + min;
    return res;
}

inline
u8 GetBoardValue(GameState *state, u32 row, u32 col)
{
    u8 res = *(state->board + (row * state->numBlocksX) + col);
    return res;
}

inline
void SetBoardValue(GameState *state, u32 row, u32 col, u8 value)
{
    *(state->board + (row * state->numBlocksX) + col) = value;
}

inline
u32 GetBoardWidth(GameState *state)
{
    u32 res = state->numBlocksX * state->blockSize;
    return res;
}

inline
u32 GetBoardHeight(GameState *state)
{
    u32 res = state->numBlocksY * state->blockSize;
    return res;
}

void GameStateReset(GameState *state)
{
    state->linesDeleted = 0;
    state->pieces = &piecesDef[0];
    state->numPieces = MF_ArrayLength(piecesDef);
    memset(state->board, 0, state->numBlocksX * state->numBlocksY);
    state->generateNewPiece();
    state->setStartRowOfPiece();
}


void GameStateInit(GameState *state)
{
    state->board = (u8 *) MF_Calloc(state->numBlocksX * state->numBlocksY);
    GameStateReset(state);
}

void GameStateDestroy(GameState *state)
{
    free(state->board);
}

void PrintPiece(Piece *piece, u8 direction)
{
    for (size_t y = 0; y < piece->dim; ++y)
    {
        for (size_t x = 0; x < piece->dim; ++x)
        {
            u8 value = piece->get(x, y, direction);
            char c = value ? 'x' : ' ';
            printf("%c", c);
        }
        printf("\n");
    }
}

void SetColor(Renderer2D &r, v3 color) {
    r.set_color(color.x, color.y, color.z, 1.0f);
}


void DrawRect(Renderer2D &r, i32 col, i32 row, u8 colorValue, u32 blockSize)
{
    float fx = col * blockSize;
    float fy = (row - 1) * blockSize;
    SetColor(r, colors[colorValue - 1]);
    r.push_rect(fx, fy, blockSize, blockSize);

    i32 thickness = 2;
    // light
    SetColor(r, colorsLight[colorValue - 1]);
    r.push_rect(fx, fy + blockSize - thickness, blockSize, thickness);

    SetColor(r, colorsLight[colorValue - 1]);
    r.push_rect(fx + blockSize - thickness, fy, thickness, blockSize);

    // dark
    SetColor(r, colorsDark[colorValue - 1]);
    r.push_rect(fx, fy, blockSize, thickness);

    SetColor(r, colorsDark[colorValue - 1]);
    r.push_rect(fx, fy, thickness, blockSize);
}


void DrawPiece(Renderer2D &r, Piece *piece, i32 col, i32 row, u8 direction, u32 amountRows, u32 blockSize)
{
    row = amountRows - row;
    for (size_t y = 0; y < piece->dim; ++y)
    {
        for (size_t x = 0; x < piece->dim; ++x)
        {
            u8 value = piece->get(x, y, direction);
            if (value)
            {
                DrawRect(r, col + x, row - y, value, blockSize);
            }
        }
    }
}

void GameState::generateNewPiece()
{
    MF_Assert(this->numPieces > 0)
    this->currentPieceIndex = GetRandomNumber(0, this->numPieces - 1);
}

void GameState::setStartRowOfPiece()
{
    Piece* piece = this->getCurrentPiece();
    //this->row = -piece->dim + piece->getRowOffset(0) + 1;
    this->row = 0;
}

bool IsPieceValid(GameState *state, i32 col, i32 row, u32 direction)
{
    Piece* piece = state->getCurrentPiece();
    for (size_t y = 0; y < piece->dim; ++y)
    {
        for (size_t x = 0;  x < piece->dim; ++x)
        {
            if (piece->get(x, y, direction) == 0)
                continue;

            u32 absCol = col + x;
            u32 absRow = row + y;

            if (absCol < 0 || absCol >= state->numBlocksX)
                return false;

            // Check bottom
            if (row > 0 && row + y >= state->numBlocksY)
                return false;

            if (absRow >= 0)
            {
                u8 boardValue = GetBoardValue(state, absRow, absCol);
                if (boardValue > 0)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

void CheckFullLines(GameState *state)
{
    u32 *foundLines = {};
    for (size_t y = 0; y < state->numBlocksY; ++y)
    {
        bool rowComplete = true;
        for (size_t x = 0; x < state->numBlocksX; ++x)
        {
            u8 value = GetBoardValue(state, y, x);
            if (value == 0)
            {
                rowComplete = false;
                break;
            }
        }

        if (rowComplete)
        {
            mf_vec_push(foundLines, (u32) y);
        }
    }

    u32 size = mf_vec_size(foundLines);
    if (size > 0)
    {
        for (i32 i = 0;
             i < size;
             ++i)
        {
            u32 lineIndex = foundLines[size - 1 - i];

            for (u32 y = lineIndex + i;
                 y > 0;
                 --y)
            {
                for (u32 x = 0;
                     x < state->numBlocksX;
                     ++x)
                {
                    SetBoardValue(state, y, x, GetBoardValue(state, y - 1, x));
                }
            }
            state->linesDeleted++;
            if (state->linesDeleted % 10 == 0 && state->tickTime > 0.1f)
            {
                state->tickTime -= 0.1f;
            }
            state->tickTime = MF_Max(state->tickTime, 0.16f);
        }
    }
    mf_vec_destroy(foundLines);
}

bool PieceAdvance(GameState *state)
{
    bool res = IsPieceValid(state, state->col, state->row + 1, state->direction);
    if (res)
    {
        state->row++;
    }
    else
    {
        if (state->row <= 2)
        {
            state->phase = GAME_OVER;
        }

        Piece* piece = state->getCurrentPiece();
        for (size_t y = 0; y < piece->dim; ++y)
        {
            for (size_t x = 0; x < piece->dim; ++x)
            {
                u8 value = piece->get(x, y, state->direction);
                if (value)
                {
                    u8 index = state->row + y;
                    SetBoardValue(state, index, state->col + x, value);
                }
            }
        }
        state->generateNewPiece();
        state->setStartRowOfPiece();
        state->row = 0;
        state->col = 0;
        state->direction = 0;

        CheckFullLines(state);
    }
    return res;
}

void GameStateUpdate(Input *input, GameState *state, float deltaSec)
{

    if (state->phase == GAME_OVER)
    {
        if (input->keys['s'].pressed)
        {
            GameStateReset(state);
            state->phase = GAME_RUNNING;
        }
        else
        {
            return;
        }
    }
    // Handle input
    if (input->keys['l'].pressed || input->keys[MF_KEY_RIGHT].pressed)
    {
        // right
        Piece* piece = state->getCurrentPiece();
        if (IsPieceValid(state, state->col + 1, state->row, state->direction))
        {
            state->col++;
        }

    }
    else if (input->keys['j'].pressed || input->keys[MF_KEY_LEFT].pressed)
    {
        if (IsPieceValid(state, state->col - 1, state->row, state->direction))
        {
            state->col--;
        }
    }

    u8 targetDirection = state->direction;
    if (input->keys['a'].pressed)
    {
        if (state->direction == 0)
        {
            targetDirection = 3;
        }
        else
        {
            targetDirection = state->direction - 1;
        }
    }

    if (input->keys['d'].pressed)
    {
        targetDirection = (state->direction + 1) % 4;
    }

    if (IsPieceValid(state, state->col, state->row, targetDirection))
    {
        state->direction = targetDirection;
    }

    if (input->keys[' '].pressed)
    {
        while (PieceAdvance(state))
        {
        }
    }

    // Process step
    state->deltaSec += deltaSec;
    if (state->deltaSec >= state->tickTime)
    {
        state->deltaSec -= state->tickTime;
        PieceAdvance(state);
    }


}

void GameStateRender(GameState* state, Renderer2D &r, u32 renderHeight)
{
    u32 width = GetBoardWidth(state);
    u32 height = GetBoardHeight(state);
    u32 borderWidth = 5;

    SetColor(r, BACKGROUND_GRAY);
    r.push_rect(0, 0, width, renderHeight);
    SetColor(r, BLACK);
    r.push_rect(-(float)borderWidth, 0, borderWidth, renderHeight);
    r.push_rect(width, 0, borderWidth, renderHeight);
    Piece* piece = &piecesDef[state->currentPieceIndex];
    DrawPiece(r, piece, state->col, state->row, state->direction, state->numBlocksY, state->blockSize);

    for (size_t y = 0; y < state->numBlocksY; ++y) {
        for (size_t x = 0; x < state->numBlocksX; ++x) {
            u8 value = GetBoardValue(state, y, x);
            if (value > 0) {
                DrawRect(r, x, state->numBlocksY - y, value, state->blockSize);
            }
        }
    }
}

struct ExampleTetris: IExample {
    GameState state;

    void init(IPlatform &p, Renderer2D &r);
    void update(IPlatform &p);
    void render(IPlatform &p, Renderer2D &r);
    void shutdown(IPlatform &p);
};

void ExampleTetris::init(IPlatform &p, Renderer2D &r) {
    viewport_bottom_up(p.window.width, p.window.height);
    GameStateInit(&this->state);

    u32 boardWidth = GetBoardWidth(&this->state);
    i32 marginLeft = (p.window.width - boardWidth) * 0.5;
    r.set_offset(marginLeft, 0.0f);
}

void ExampleTetris::update(IPlatform &p) {
    GameStateUpdate(&p.input, &this->state, p.timer.deltaSec);
}

void ExampleTetris::render(IPlatform &p, Renderer2D &r) {
    SetColor(r, BACKGROUND_LIGHT);
    r.push_clear();
    GameStateRender(&state, r, p.window.height);
}

void ExampleTetris::shutdown(IPlatform &p) {
    GameStateDestroy(&this->state);
}

}
