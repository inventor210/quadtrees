// **************************************************************
// *		   
// *  quadtree.cpp
// *		   
// *  Quadtree class
// *		   
// *  CS 225 Spring 2008
// *		   
// **************************************************************

#include "quadtree.h"

/*
 * Default Quadtree constructor with no parameters
 */
Quadtree::Quadtree()
{
	root = NULL;
	width = 0;
	height = 0;
}

/*
 * Two argument Quadtree constructor
 * parameters: const PNG & source, int resolution
 */
Quadtree::Quadtree(const PNG & source, int resolution)
{
	root = NULL;
	width = resolution;
	height = resolution;
	buildTree(source, resolution);
}

/*
 * Quadtree copy constructor
 */
Quadtree::Quadtree(Quadtree const & other)
{
	root = copy(other.root);
	width = other.width;
	height = other.height;
}

/*
 *	Copies a Quadtree
 */
Quadtree const & Quadtree::operator=(Quadtree const & rhs)
{
	if(this != &rhs)
	{
		clear();
		root = copy(rhs.root);
		width = rhs.width;
		height = rhs.width;
	}
	return *this;
}

/*
 * Quadtree destructor
 */
Quadtree::~Quadtree()
{
	clear();
}


/*
 * Clears current Quadtree
 * Represents upper dxd block of source
 */
void Quadtree::buildTree(const PNG & source, int d)
{
	clear();
	buildTree(source, 0, 0, d, root);
	width = d;
	height = d;
}
/*
 * buildTree helper
 */
void Quadtree::buildTree(const PNG & source, int x, int y, int d, QuadtreeNode* & r)
{
	if(d==1)
	{
		QuadtreeNode * node = new QuadtreeNode();
		node->element = *source(x,y);
		r = node;
		return;
	}

	QuadtreeNode * node = new QuadtreeNode();
	r = node;
	buildTree(source, x, y, d/2, r->nwChild);
	buildTree(source, x + (d/2), y, d/2, r->neChild);
	buildTree(source, x, y + (d/2), d/2, r->swChild);
	buildTree(source, x + (d/2), y + (d/2), d/2, r->seChild);

	if(r->nwChild != NULL && r->neChild != NULL && r->swChild != NULL && r->seChild != 					NULL)
	{
		r->element.red = (r->nwChild->element.red + r->neChild->element.red + 
			r->swChild->element.red + r->seChild->element.red) / 4;
		r->element.green = (r->nwChild->element.green + r->neChild->element.green + 
			r->swChild->element.green + r->seChild->element.green) / 4;
		r->element.blue = (r->nwChild->element.blue + r->neChild->element.blue + 
			r->swChild->element.blue + r->seChild->element.blue) / 4;
	}
}

/*
 * Grabs a the pixel at coordinates (x,y) in the bitmap image
 * and returns it
 */
RGBAPixel Quadtree::getPixel(int x, int y)
{
	if(root != NULL)
		return getPixel_helper(x, y, root, width, height);
	return RGBAPixel();
}
/*
 * get pixel helper
 */
RGBAPixel Quadtree::getPixel_helper(int x, int y, QuadtreeNode * & r, int xcoor, int ycoor)
{
	if(r->nwChild ==NULL && r->neChild == NULL && r->swChild == NULL && r->seChild== 					NULL)
		return r->element;
	if(x < xcoor/2 && y < ycoor/2 && r->nwChild != NULL)
		return getPixel_helper(x, y, r->nwChild, xcoor/2, ycoor/2);
	else if(x >= xcoor/2 && y < ycoor/2 && r->neChild != NULL)
		return getPixel_helper(x - xcoor/2, y, r->neChild, xcoor/2, ycoor/2);
	else if(x < xcoor/2 && y >= ycoor/2 && r->swChild != NULL)
		return getPixel_helper(x, y-ycoor/2, r->swChild, xcoor/2, ycoor/2);
	else if(x >= xcoor/2 && y >= ycoor/2 && r->seChild != NULL)
		return getPixel_helper(x-xcoor/2, y-ycoor/2, r->seChild, xcoor/2, ycoor/2);
	else
		 return r->element;
}
/*
 * returns the underlying image represented by the Quadtree
 */
PNG Quadtree::decompress()
{
	PNG result;
	result.resize(width, height);
	for(int i = 0; i < result.width(); i++)
	{
		for(int j = 0; j < result.height(); j++)
		{
			*result(i,j) = getPixel(i, j);
		}
	}
	return result;
}


/*
 * rotates the underlying image by 90 degrees
 */
void Quadtree::clockwiseRotate()
{
	if(root != NULL)
		clockwiseRotate(root);
}

/*
 * private helper fucntion for clockwise rotate
 */
void Quadtree::clockwiseRotate(QuadtreeNode * & r)
{
	if(r != NULL)
	{

		QuadtreeNode * t;
		t = r->nwChild;
		r->nwChild = r->swChild;
		r->swChild = r->seChild;
		r->seChild = r->neChild;
		r->neChild = t; 
		t = NULL;
	
		clockwiseRotate(r->nwChild);
		clockwiseRotate(r->neChild);
		clockwiseRotate(r->swChild);
		clockwiseRotate(r->seChild);

	}
}


/*
 * returns number of leaves the tree would have if pruned
 */
int Quadtree::pruneSize(int tolerance) const
{
	Quadtree t(*this);
	t.prune(tolerance);
	return t.count();
}

/*
 * Calculates and returns the minimum tolerance necessary to guarantee that upon pruning the tree
 */
int Quadtree::idealPrune(int numLeaves)
{
	int min = 0;
	int max = 255*255 + 255*255 + 255*255;
	return idealPrune(numLeaves, min, max);
}
/*
 * private helper for idealPrune
 */
int Quadtree::idealPrune(int numLeaves, int min, int max)
{
	int mid = (min + max)/2;
	int size = pruneSize(mid);

	if(mid == min || mid == max)
		return mid+1;
	if(size<=numLeaves)
		return idealPrune(numLeaves, min, mid);
	return idealPrune(numLeaves, mid, max);
}
/*
 * compresses the image that the quadtree represents
 */
void Quadtree::prune(int tolerance)
{
	if(root != NULL)
		prune(tolerance, root);
}
/*
 * private helper for prune
 */
void Quadtree::prune(int tol, QuadtreeNode * & r)
{
	if(r == NULL || (r->nwChild == NULL && r->neChild == NULL && r->swChild == NULL && r->seChild == 				NULL))
		return;

	bool nwPrune = canPrune(tol, r, r->nwChild);
	bool nePrune = canPrune(tol, r, r->neChild);
	bool swPrune = canPrune(tol, r, r->swChild);
	bool sePrune = canPrune(tol, r, r->seChild);

	if(nwPrune && nePrune && swPrune && sePrune)
	{
		clear(r->nwChild);
		clear(r->neChild);
		clear(r->swChild);
		clear(r->seChild);
	}
	else
	{
		prune(tol, r->nwChild);
		prune(tol, r->neChild);
		prune(tol, r->seChild);
		prune(tol, r->swChild);
	}
}

bool Quadtree::canPrune(int tol, QuadtreeNode* & r1, QuadtreeNode* & r2)
{
	if(r2->nwChild == NULL)
	{
		RGBAPixel pix1 = r1->element;
		RGBAPixel pix2 = r2->element;
		
		int red1 = pix1.red;
		int red2 = pix2.red;
		int green1 = pix1.green;
		int green2 = pix2.green;
		int blue1 = pix1.blue;
		int blue2 = pix2.blue;

		int difference = (red2 - red1)*(red2-red1) + (green2 - green1)*(green2 - green1) + (blue2 - 				blue1)*(blue2 - blue1);

		return (difference <= tol);
	}

	bool nwPrune = canPrune(tol, r1, r2->nwChild);
	bool nePrune = canPrune(tol, r1, r2->neChild);
	bool swPrune = canPrune(tol, r1, r2->swChild);
	bool sePrune = canPrune(tol, r1, r2->seChild);

	return(nwPrune && nePrune && swPrune && sePrune);
}

/*
 * private helper function to copy a Quadtree
 */
Quadtree::QuadtreeNode* Quadtree::copy(QuadtreeNode * other)
{
	QuadtreeNode * result = NULL;
	if(other != NULL)
	{
		result = new QuadtreeNode(other->element);
		result->nwChild = copy(other->nwChild);
		result->neChild = copy(other->neChild);
		result->swChild = copy(other->swChild);
		result->seChild = copy(other->seChild);
	}
	return result;
}

void Quadtree::clear()
{
	clear(root);
	width = 0;
	height = 0;
}
/*
 * private helper function to clear a Quadtree
 */
void Quadtree::clear(QuadtreeNode * & r)
{
	if(r != NULL)
	{
			clear(r->nwChild);
			clear(r->neChild);
			clear(r->swChild);
			clear(r->seChild);
			delete r;
			r = NULL;
	}
}

int Quadtree::count()
{
	int start = 0;
	return count(root, start);
}
int Quadtree::count(QuadtreeNode * r, int & c)
{
	if(r->nwChild == NULL && r->neChild == NULL && r->swChild == NULL && 
			r->seChild == NULL)
		return 1;
	c = count(r->nwChild, c) + count(r->neChild, c) + count(r->swChild, c) + count(r->seChild, c);
	return c; 
}
