#include "structures.h"

#include "log.h"
#include "memory.h"

QuadTree* createQuadTree(int level, Rect bounds)
{
    QuadTree* quadTree = checkedMalloc(sizeof(QuadTree));
    quadTree->level = level;
    quadTree->bounds = bounds;
    quadTree->objCount = 0;
    
    for (int i = 0; i < 4; i++)
        quadTree->nodes[i] = NULL;

    for (int i = 0; i < MAX_OBJECTS; i++)
        quadTree->objects[i] = NULL;

    return quadTree;
}

void split(QuadTree* quadTree)
{
    float subWidth = quadTree->bounds.width / 2.0f;
    float subHeight = quadTree->bounds.height / 2.0f;
    float x = quadTree->bounds.position.x;
    float y = quadTree->bounds.position.y;

    quadTree->nodes[TOP_LEFT_QUADRANT] = createQuadTree(quadTree->level + 1, (Rect) { 
        { .x = x, .y = y },
        .width = subWidth,
        .height = subHeight 
    });

    quadTree->nodes[TOP_RIGHT_QUADRANT] = createQuadTree(quadTree->level + 1, (Rect) {
        { .x = x + subWidth, .y = y },
        .width = subWidth,
        .height = subHeight 
    });

    quadTree->nodes[BOTTOM_RIGHT_QUADRANT] = createQuadTree(quadTree->level + 1, (Rect) {
        { .x = x + subWidth, .y = y - subHeight },
        .width = subWidth,
        .height = subHeight
    });

    quadTree->nodes[BOTTOM_LEFT_QUADRANT] = createQuadTree(quadTree->level + 1, (Rect) {
        { .x = x, .y = y - subHeight },
        .width = subWidth,
        .height = subHeight
    });
}

size_t getIndex(Rect bounds, Block* object)
{
    size_t index = INVALID_INDEX;

    float verticalMidpoint = bounds.position.x + (bounds.width / 2.0f);
    float horizontalMidpoint = bounds.position.y + (bounds.height / 2.0f);

    // Object can completely fit within the top quadrants
    bool topQuadrant = (object->position.y < horizontalMidpoint 
        && object->position.y + object->height < horizontalMidpoint);
    // Object can completely fit within the bottom quadrants
    bool bottomQuadrant = (object->position.y > horizontalMidpoint);

    // Object can completely fit within the left quadrants
    if (object->position.x < verticalMidpoint && object->position.x + object->width < verticalMidpoint)
    {
        if (topQuadrant)
            index = TOP_LEFT_QUADRANT;
        else if (bottomQuadrant)
            index = BOTTOM_LEFT_QUADRANT;
    }
    // Object can completely fit within the right quadrants
    else if (object->position.x > verticalMidpoint)
    {
        if (topQuadrant)
            index = TOP_RIGHT_QUADRANT;
        else if (bottomQuadrant)
            index = BOTTOM_RIGHT_QUADRANT;
    }

    return index;
}

void insert(QuadTree* quadTree, Block* object)
{
    if (quadTree->nodes[0] != NULL)
    {
        size_t index = getIndex(quadTree->bounds, object);
        if (index != INVALID_INDEX)
        {
            insert(quadTree->nodes[index], object);
            quadTree->objCount++;
            return;
        }
    }

    // Jeśli lista obiektów jest pełna, dzielimy obszar na podobszary
    if (quadTree->objects[MAX_OBJECTS - 1] != NULL)
    {
        // Dzielimy obszar na podobszary
        split(quadTree);

        // Przeszukujemy wszystkie obiekty na liście
        for (size_t i = 0; i < MAX_OBJECTS; i++)
        {
            // Rekurencyjnie wstawiamy klon obiektu do odpowiedniego podobszaru
            size_t index = getIndex(quadTree->bounds, quadTree->objects[i]);
            if (index != INVALID_INDEX)
            {
                // TODO: add blocks to an array instead of mallocing every time :(
                Block* object_clone = checkedMalloc(sizeof(Block));
                *object_clone = *quadTree->objects[i]; // Klonujemy obiekt
                insert(quadTree->nodes[index], object_clone);
            }
            quadTree->objects[i] = NULL; // Czyścimy referencję do obiektu z listy
        }
    }

    // Dodajemy klon nowego obiektu do listy w bieżącym węźle
    for (int i = 0; i < MAX_OBJECTS; i++)
    {
        if (quadTree->objects[i] == NULL)
        {
            Block* object_clone = checkedMalloc(sizeof(Block));
            *object_clone = *object; // Klonujemy obiekt
            quadTree->objects[i] = object_clone;
            quadTree->objCount++;
            break;
        }
    }
}

void retrieve(QuadTree* quadTree, Block* object)
{
    size_t index = getIndex(quadTree->bounds, object);

    if (index != INVALID_INDEX && quadTree->nodes[0] != NULL)
        retrieve(quadTree->nodes[index], object);
}

void display(QuadTree* quadTree)
{
    logNotification("quadTree bounds: (%.2lf, %.2lf, %.2lf, %.2lf)\n",
        (double)quadTree->bounds.position.x, (double)quadTree->bounds.position.y,
        (double)quadTree->bounds.width, (double)quadTree->bounds.height);

    for (size_t i = 0; i < MAX_OBJECTS; i++)
    {
        if (quadTree->objects[i] != NULL)
        {
            logNotification("Object at (%.2lf, %.2lf, %.2lf, %.2lf)\n",
                (double)quadTree->objects[i]->position.x, (double)quadTree->objects[i]->position.y,
                (double)quadTree->objects[i]->width, (double)quadTree->objects[i]->height);
        }
    }

    for (size_t i = 0; i < 4; i++)
    {
        if (quadTree->nodes[i] != NULL)
            display(quadTree->nodes[i]);
    }
}

void removeBlock(QuadTree* quadTree, Block* block)
{
    size_t index = getIndex(quadTree->bounds, block);

    if (index != INVALID_INDEX && quadTree->nodes[0] != NULL)
    {
        removeBlock(quadTree->nodes[index], block);
    }
    else
    {
        for (size_t i = 0; i < MAX_OBJECTS; i++)
        {
            if (quadTree->objects[i] == block)
            {
                free(quadTree->objects[i]);
                quadTree->objects[i] = NULL;
                quadTree->objCount--;
                return;
            }
        }
    }
    
    quadTree->objCount--;
}

void retrieveBlocks(QuadTree* quadTree, Vec2 position, Block** blocks, size_t* count)
{
    size_t index;

    {
        Block tmp = { .position = position, .width = 0.0f, .height = 0.0f };
        index = getIndex(quadTree->bounds, &tmp);
    }

    if (index != INVALID_INDEX && quadTree->nodes[0] != NULL)
    {
        retrieveBlocks(quadTree->nodes[index], position, blocks, count);
    }
    else
    {
        for (size_t i = 0; i < MAX_OBJECTS; i++)
        {
            if (quadTree->objects[i] != NULL)
            {
                blocks[*count] = quadTree->objects[i];
                (*count)++;
            }
        }
    }
}

static Block* retrieveNthImpl(const QuadTree* quadTree, size_t index, size_t* current)
{
    for (size_t i = 0; i < MAX_OBJECTS; i++)
    {
        if (quadTree->objects[i] != NULL)
        {
            if (*current == index)
                return quadTree->objects[i];
            else
                (*current)++;
        }
    }

    for (size_t i = 0; i < 4; i++)
    {
        if (quadTree->nodes[i] != NULL)
        {
            Block* result = retrieveNthImpl(quadTree->nodes[i], index, current);

            if (result)
                return result;
        }
    }
    
    return NULL;
}

Block* retrieveNth(const QuadTree* quadTree, size_t index)
{
    size_t current = 0;
    return retrieveNthImpl(quadTree, index, &current);
}