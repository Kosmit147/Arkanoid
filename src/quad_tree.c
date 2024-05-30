#include "quad_tree.h"

#include <stdbool.h>

#include "memory.h"

#define TOP_LEFT_QUADRANT_INDEX 0
#define TOP_RIGHT_QUADRANT_INDEX 1
#define BOTTOM_RIGHT_QUADRANT_INDEX 2
#define BOTTOM_LEFT_QUADRANT_INDEX 3

#define TOP_LEFT_QUADRANT_BIT       0x8
#define TOP_RIGHT_QUADRANT_BIT      0x4
#define BOTTOM_RIGHT_QUADRANT_BIT   0x2
#define BOTTOM_LEFT_QUADRANT_BIT    0x1

#define NO_QUADRANTS                0x0
#define ALL_QUADRANTS               0xF

static QuadTreeNode* createQuadTreeNode(RectBounds bounds)
{
    QuadTreeNode* node = checkedMalloc(sizeof(QuadTreeNode));

    *node = (QuadTreeNode) {
        .bounds = bounds,
    };

    for (size_t i = 0; i < 4; i++)
        node->nodes[i] = NULL;

    for (size_t i = 0; i < MAX_QUAD_TREE_NODE_BLOCKS; i++)
        node->blocks[i] = NULL;

    return node;
}

static void splitQuadTreeNode(QuadTreeNode* node)
{
    Vec2 topLeft = node->bounds.topLeft;
    Vec2 bottomRight = node->bounds.bottomRight;
    Vec2 middlePoint = (Vec2) {
        .x = (topLeft.x + bottomRight.x) / 2.0f,
        .y = (topLeft.y + bottomRight.y) / 2.0f,
    };

    node->nodes[TOP_LEFT_QUADRANT_INDEX] = createQuadTreeNode((RectBounds) {
        .topLeft = topLeft,
        .bottomRight = middlePoint,
    });

    node->nodes[TOP_RIGHT_QUADRANT_INDEX] = createQuadTreeNode((RectBounds) {
        .topLeft = (Vec2) { .x = middlePoint.x, .y = topLeft.y, },
        .bottomRight = (Vec2) { .x = bottomRight.x, .y = middlePoint.y, },
    });

    node->nodes[BOTTOM_RIGHT_QUADRANT_INDEX] = createQuadTreeNode((RectBounds) {
        .topLeft = middlePoint,
        .bottomRight = bottomRight,
    });

    node->nodes[BOTTOM_LEFT_QUADRANT_INDEX] = createQuadTreeNode((RectBounds) {
        .topLeft = (Vec2) { .x = topLeft.x, .y = middlePoint.y, },
        .bottomRight = (Vec2) { .x = middlePoint.x, .y = bottomRight.y, },
    });
}

static uint8_t getNodeQuadrantsByBounds(const QuadTreeNode* node, const RectBounds* bounds)
{
    uint8_t quadrants = NO_QUADRANTS;

    Vec2 midpoint = {
        .x = (node->bounds.topLeft.x + node->bounds.bottomRight.x) / 2.0f,
        .y = (node->bounds.topLeft.y + node->bounds.bottomRight.y) / 2.0f,
    };

    bool topQuadrant = bounds->topLeft.y > midpoint.y;
    bool bottomQuadrant = bounds->bottomRight.y < midpoint.y;

    // blockBounds overlays left quadrant
    if (bounds->topLeft.x < midpoint.x)
    {
        if (topQuadrant)
            quadrants |= TOP_LEFT_QUADRANT_BIT;

        if (bottomQuadrant)
            quadrants |= BOTTOM_LEFT_QUADRANT_BIT;
    }

    // blockBounds overlays right quadrant
    if (bounds->bottomRight.x > midpoint.x)
    {
        if (topQuadrant)
            quadrants |= TOP_RIGHT_QUADRANT_BIT;

        if (bottomQuadrant)
            quadrants |= BOTTOM_RIGHT_QUADRANT_BIT;
    }

    return quadrants;
}
static uint8_t getNodeQuadrantsForBlock(const QuadTreeNode* node, const Block* block)
{
    RectBounds blockBounds = getBlockRectBounds(block);
    return getNodeQuadrantsByBounds(node, &blockBounds);
}

QuadTree quadTreeCreate(RectBounds bounds)
{
    QuadTreeNode* root = createQuadTreeNode(bounds);

    return (QuadTree) {
        .root = root,
        .elemCount = 0,
    };
}

static void quadTreeInsertImpl(QuadTreeNode* node, const Block* block);

static inline void insertIntoQuadrants(QuadTreeNode* node, const Block* block, uint8_t quadrants)
{
    if (quadrants & TOP_LEFT_QUADRANT_BIT)
        quadTreeInsertImpl(node->nodes[TOP_LEFT_QUADRANT_INDEX], block);

    if (quadrants & TOP_RIGHT_QUADRANT_BIT)
        quadTreeInsertImpl(node->nodes[TOP_RIGHT_QUADRANT_INDEX], block);

    if (quadrants & BOTTOM_RIGHT_QUADRANT_BIT)
        quadTreeInsertImpl(node->nodes[BOTTOM_RIGHT_QUADRANT_INDEX], block);

    if (quadrants & BOTTOM_LEFT_QUADRANT_BIT)
        quadTreeInsertImpl(node->nodes[BOTTOM_LEFT_QUADRANT_INDEX], block);
}

static void addBlockToNode(QuadTreeNode* node, const Block* block)
{
    for (size_t i = 0; i < MAX_QUAD_TREE_NODE_BLOCKS; i++)
    {
        if (node->blocks[i] == NULL)
        {
            node->blocks[i] = block;
            return;
        }
    }
}

static bool blockInserted = false;

static void quadTreeInsertImpl(QuadTreeNode* node, const Block* block)
{
    if (quadTreeNodeHasSubnodes(node))
    {
        uint8_t quadrants = getNodeQuadrantsForBlock(node, block);
        insertIntoQuadrants(node, block, quadrants);
        return;
    }

    if (quadTreeNodeFull(node))
    {
        splitQuadTreeNode(node);

        for (size_t i = 0; i < MAX_QUAD_TREE_NODE_BLOCKS; i++)
        {
            quadTreeInsertImpl(node, node->blocks[i]);
            node->blocks[i] = NULL;
        }

        uint8_t quadrants = getNodeQuadrantsForBlock(node, block);
        insertIntoQuadrants(node, block, quadrants);
        return;
    }

    addBlockToNode(node, block);
    blockInserted = true;
}

void quadTreeInsert(QuadTree* quadTree, const Block* block)
{
    blockInserted = false;
    quadTreeInsertImpl(quadTree->root, block);

    if (blockInserted)
        quadTree->elemCount++;
}

static bool blockRemoved = false;

void quadTreeRemoveBlockImpl(QuadTreeNode* node, const Block* block)
{
    if (quadTreeNodeHasSubnodes(node))
    {
        uint8_t quadrants = getNodeQuadrantsForBlock(node, block);

        if (quadrants & TOP_LEFT_QUADRANT_BIT)
            quadTreeRemoveBlockImpl(node->nodes[TOP_LEFT_QUADRANT_INDEX], block);

        if (quadrants & TOP_RIGHT_QUADRANT_BIT)
            quadTreeRemoveBlockImpl(node->nodes[TOP_RIGHT_QUADRANT_INDEX], block);

        if (quadrants & BOTTOM_RIGHT_QUADRANT_BIT)
            quadTreeRemoveBlockImpl(node->nodes[BOTTOM_RIGHT_QUADRANT_INDEX], block);

        if (quadrants & BOTTOM_LEFT_QUADRANT_BIT)
            quadTreeRemoveBlockImpl(node->nodes[BOTTOM_LEFT_QUADRANT_INDEX], block);

        return;
    }

    for (size_t i = 0; i < MAX_QUAD_TREE_NODE_BLOCKS - 1; i++)
    {
        if (node->blocks[i] == block)
        {
            eraseFromArr(node->blocks, i, MAX_QUAD_TREE_NODE_BLOCKS, sizeof(const Block*));
            node->blocks[MAX_QUAD_TREE_NODE_BLOCKS - 1] = NULL;
            blockRemoved = true;
            return;
        }
    }

    if (node->blocks[MAX_QUAD_TREE_NODE_BLOCKS - 1] == block)
    {
        node->blocks[MAX_QUAD_TREE_NODE_BLOCKS - 1] = NULL;
        blockRemoved = true;
        return;
    }
}

void quadTreeRemoveBlock(QuadTree* quadTree, const Block* block)
{
    blockRemoved = false;
    quadTreeRemoveBlockImpl(quadTree->root, block);

    if (blockRemoved)
        quadTree->elemCount--;
}

static void quadTreeRetrieveAllByBoundsImpl(const QuadTreeNode* node, RectBounds bounds, Vector* result)
{
    if (quadTreeNodeHasSubnodes(node))
    {
        uint8_t quadrants = getNodeQuadrantsByBounds(node, &bounds);

        if (quadrants & TOP_LEFT_QUADRANT_BIT)
            quadTreeRetrieveAllByBoundsImpl(node->nodes[TOP_LEFT_QUADRANT_INDEX], bounds, result);

        if (quadrants & TOP_RIGHT_QUADRANT_BIT)
            quadTreeRetrieveAllByBoundsImpl(node->nodes[TOP_RIGHT_QUADRANT_INDEX], bounds, result);

        if (quadrants & BOTTOM_RIGHT_QUADRANT_BIT)
            quadTreeRetrieveAllByBoundsImpl(node->nodes[BOTTOM_RIGHT_QUADRANT_INDEX], bounds, result);

        if (quadrants & BOTTOM_LEFT_QUADRANT_BIT)
            quadTreeRetrieveAllByBoundsImpl(node->nodes[BOTTOM_LEFT_QUADRANT_INDEX], bounds, result);

        return;
    }

    for (size_t i = 0; i < MAX_QUAD_TREE_NODE_BLOCKS; i++)
    {
        if (node->blocks[i] == NULL)
            break;

        bool foundDuplicate = false;

        for (size_t j = 0; j < vectorSize(result, sizeof(const Block*)); j++)
        {
            const Block* tmp = *(const Block**)vectorGet(result, j, sizeof(const Block*));

            if (tmp == node->blocks[i])
            {
                foundDuplicate = true;
                break;
            }
        }

        if (!foundDuplicate)
            vectorPushBack(result, &node->blocks[i], sizeof(const Block*));
    }
}

void quadTreeFreeImpl(QuadTreeNode* node)
{
    if (quadTreeNodeHasSubnodes(node))
        for (size_t i = 0; i < 4; i++)
            quadTreeFreeImpl(node->nodes[i]);

    free(node);
}

void quadTreeFree(QuadTree* quadTree)
{
    quadTreeFreeImpl(quadTree->root);
    quadTree->root = NULL;
    quadTree->elemCount = 0;
}

void quadTreeRetrieveAllByBounds(const QuadTree* quadTree, RectBounds bounds, Vector* result)
{
    quadTreeRetrieveAllByBoundsImpl(quadTree->root, bounds, result);
}
