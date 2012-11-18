#include <stdlib.h>
#include "octree.h"


OctreeNode3f* OctreeNode3f_malloc(Vector3f bounds1, Vector3f bounds2)
{
  OctreeNode3f *node = malloc(sizeof(node));
  if (!node)
    return NULL;
  /* Calculate the bounds of the node, ensuring that the minimal x, y, and z 
   values are in bounds_bot, and the maximal are in bounds_top */
  
  Vector3f bottom = {(bounds1.x < bounds2.x ? bounds1.x : bounds2.x),
		     (bounds1.y < bounds2.y ? bounds1.y : bounds2.y),
		     (bounds1.z < bounds2.z ? bounds1.z : bounds2.z)};
  node->bounds_bot = bottom;
  Vector3f top = {(bounds1.x < bounds2.x ? bounds2.x : bounds1.x),
		  (bounds1.y < bounds2.y ? bounds2.y : bounds1.y),
		  (bounds1.z < bounds2.z ? bounds2.z : bounds1.z)};
  node->bounds_top = top;
  Vector3f middle = {(bounds1.x+bounds2.x)/2,
		     (bounds1.y+bounds2.y)/2,
		     (bounds1.z+bounds2.z)/2};
  node->bounds_mid = middle;
  /* Zero-NULL all the other attributes */
  node->position = NULL;
  node->children[0] = NULL;
  node->children[1] = NULL;
  node->children[2] = NULL;
  node->children[3] = NULL;
  node->children[4] = NULL;
  node->children[5] = NULL;
  node->children[6] = NULL;
  node->children[7] = NULL;
  node->elements = 0;
  node->usr_val = NULL;
  return node;
}


void OctreeNode3f_free(OctreeNode3f *node) {
  if (!node) return;
  free(node->position);
  node->usr_val = NULL;
  OctreeNode3f_free(node->children[0]);
  OctreeNode3f_free(node->children[1]);
  OctreeNode3f_free(node->children[2]);
  OctreeNode3f_free(node->children[3]);
  OctreeNode3f_free(node->children[4]);
  OctreeNode3f_free(node->children[5]);
  OctreeNode3f_free(node->children[6]);
  OctreeNode3f_free(node->children[7]);
  free(node);
}


int OctreeNode3f__insert_sub(OctreeNode3f*, Vector3f, void*);

int OctreeNode3f_insert(OctreeNode3f *node, Vector3f pos, void *usr_val) {
  if (!node) return 0;
  if (node->elements == 0) {
    node->position = malloc(sizeof(Vector3f));
    if (!(node->position))
      return 0;
    node->position->x = pos.x;
    node->position->y = pos.y;
    node->position->z = pos.z;
    node->usr_val = usr_val;
  } else {
    if (node->elements == 1) {
      /* If this node is a leaf, take its position and place it in the 
       appropriate child, no longer making this a leaf */
      OctreeNode3f__insert_sub(node, *(node->position), node->usr_val);
      free(node->position);
      node->position = NULL;
      node->usr_val = NULL;
    }
    OctreeNode3f__insert_sub(node, pos, usr_val);
  }
  (node->elements)++;
  return (node->elements);
}
/* *** HELPER ***
 * Inserts a value into the appropriate subnode of the specified node
 * RETURNS - 0 on failure
 *           The new count of elements in the subnode that was added to */
int OctreeNode3f__insert_sub(OctreeNode3f *node, Vector3f pos, void *usr_val) {
  if (!node) return 0;
  int sub = 0;
  Vector3f min = {0,0,0};
  Vector3f max = {0,0,0};
  if (pos.x > node->bounds_mid.x) {
    sub += 1;
    min.x = node->bounds_mid.x;
    max.x = node->bounds_top.x;
  } else {
    min.x = node->bounds_bot.x;
    max.x = node->bounds_mid.x;
  }
  if (pos.y > node->bounds_mid.y) {
    sub += 2;
    min.y = node->bounds_mid.y;
    max.y = node->bounds_top.y;
  } else {
    min.y = node->bounds_bot.y;
    max.y = node->bounds_mid.y;
  }
  if (pos.z > node->bounds_mid.z) {
    sub += 4;
    min.z = node->bounds_mid.z;
    max.z = node->bounds_top.z;
  } else {
    min.z = node->bounds_bot.z;
    max.z = node->bounds_mid.z;
  }
  (node->children)[sub] = OctreeNode3f_malloc(min, max);
  // (the next line naturally checks for a successful malloc)
  return OctreeNode3f_insert((node->children)[sub], pos, usr_val);
}


int OctreeNode3f_preorder(OctreeNode3f *node, void (*f_node)(OctreeNode3f*), void (*f_leaf)(OctreeNode3f*)) {
  if (!node) return 0;
  if (node->elements == 1) {
    if (f_leaf)
      (*f_leaf)(node);
  } else {
    if (f_node)
      (*f_node)(node);
  }
  OctreeNode3f_preorder(node->children[0], f_node, f_leaf);
  OctreeNode3f_preorder(node->children[1], f_node, f_leaf);
  OctreeNode3f_preorder(node->children[2], f_node, f_leaf);
  OctreeNode3f_preorder(node->children[3], f_node, f_leaf);
  OctreeNode3f_preorder(node->children[4], f_node, f_leaf);
  OctreeNode3f_preorder(node->children[5], f_node, f_leaf);
  OctreeNode3f_preorder(node->children[6], f_node, f_leaf);
  OctreeNode3f_preorder(node->children[7], f_node, f_leaf);
  return 1;
}

int OctreeNode3f_postorder(OctreeNode3f *node, void (*f_node)(OctreeNode3f*), void (*f_leaf)(OctreeNode3f*)) {
  if (!node) return 0;
  OctreeNode3f_postorder(node->children[0], f_node, f_leaf);
  OctreeNode3f_postorder(node->children[1], f_node, f_leaf);
  OctreeNode3f_postorder(node->children[2], f_node, f_leaf);
  OctreeNode3f_postorder(node->children[3], f_node, f_leaf);
  OctreeNode3f_postorder(node->children[4], f_node, f_leaf);
  OctreeNode3f_postorder(node->children[5], f_node, f_leaf);
  OctreeNode3f_postorder(node->children[6], f_node, f_leaf);
  OctreeNode3f_postorder(node->children[7], f_node, f_leaf);
  if (node->elements == 1) {
    if (f_leaf)
      (*f_leaf)(node);
  } else {
    if (f_node)
      (*f_node)(node);
  }
  return 1;
}