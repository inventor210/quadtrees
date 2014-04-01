// **************************************************************
// *		   
// *  quadtree.h
// *		   
// *  Quadtree class
// *		   
// *  CS 225 Spring 2008
// *		   
// **************************************************************

#ifndef QUADTREE_H
#define QUADTREE_H

#include <iostream>
#include "png.h"

using namespace std;
class Quadtree
{
	public:
	
	/*
	 * Default Quadtree constructor with no parameters
	 */
	Quadtree();

	/*
	 * Two argument Quadtree constructor
	 * parameters: const PNG & source, int resolution
	 */
	Quadtree(const PNG & source, int resolution);

	/*
	 * Quadtree copy constructor
	 */
	Quadtree(Quadtree const & other);

	/*
	 *	Copies a Quadtree
	 */
	Quadtree const & operator=(Quadtree const & rhs);

	/*
 	 * Quadtree destructor
 	 */
	~Quadtree();
	
	/*
	 * Clears current Quadtree
	 * Represents upper dxd block of source
	 */
	void buildTree(const PNG & source, int d);

	/*
	 * Grabs a the pixel at coordinates (x,y) in the bitmap image
	 * and returns it
	 */
	RGBAPixel getPixel(int x, int y);

	/*
	 * returns the underlying image represented by the Quadtree
	 */
	PNG decompress();

	/*
	 * Rotates the image 90 degrees
	 */
	void clockwiseRotate();

	/*
	 * compresses the image that the quadtree represents
	 */
	void prune(int tolerance);

	/*
	 * returns the number of leaves the quadtree would have if pruned
	 */
	int pruneSize(int tolerance) const;

	/*
	 * Calculates and returns the minimum tolerance necessary to guarantee that upon pruning the tree
	 */
	int idealPrune(int numLeaves);


	int count();

	


	private:

	// A simple class representing a single node of a Quadtree.
	// You may want to add to this class; in particular, it could probably
	// use a constructor or two...
	class QuadtreeNode
	{
		public:
		QuadtreeNode* nwChild;  // pointer to northwest child
		QuadtreeNode* neChild;  // pointer to northeast child
		QuadtreeNode* swChild;  // pointer to southwest child
		QuadtreeNode* seChild;  // pointer to southeast child
	

		RGBAPixel element;  // the pixel stored as this node's "data"
		
		QuadtreeNode()
		{
			nwChild = NULL;
			neChild = NULL;
			swChild = NULL;
			seChild = NULL;
		}

		/*
		 * QuadtreeNode constructor
		 */
		QuadtreeNode(const RGBAPixel color)
		{
			element = color;
			nwChild = NULL;
			neChild = NULL;
			swChild = NULL;
			seChild = NULL;
		}
			
	};

	/*
 	 * private helper function to clear a Quadtree
 	 */
	void clear(QuadtreeNode * &r);
	void clear();
	/*
	 * private helper function to copy a Quadtree
	 */
	QuadtreeNode* copy(QuadtreeNode * root);

	// private helper for buildTree
	void buildTree(const PNG & source, int x, int y, int d, QuadtreeNode* & r);

    // private helper for prune
	void prune(int tol, QuadtreeNode * & r);

	// private function, returns if a subtree can be pruned
	bool canPrune(int tol, QuadtreeNode* & r1, QuadtreeNode* & r2);

	// private helper for idealPrune
	int idealPrune(int numLeaves, int min, int max);
	/*
	 * private helper fucntion for clockwise rotate
	 */
	void clockwiseRotate(QuadtreeNode * & r);

	// private helper for getPixel
	RGBAPixel getPixel_helper(int x, int y, QuadtreeNode * & r, int xcoor, int ycoor);

	// counts number of nodes
	int count(QuadtreeNode * r, int & c);
	

	QuadtreeNode* root;    // pointer to root of quadtree
	
	int width;
	int height;
	
	
	/**** Functions added for testing/grading                ****/
	/**** Do not remove this line or copy its contents here! ****/
	#include "quadtree_given.h"
};

#endif
