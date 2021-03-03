#pragma once
#ifndef _KDTREENODEMAKER_H
#define _KDTREENODEMAKER_H

#include "Fixed_Data.h"
#include "pch.h"

enum cutType {
	VERTICAL = 100,
	HORIZONTAL,
	LEAF,
	INITIAL_CUT = VERTICAL
};

class KD_Node
{
public:
	//constructors and destructors
	KD_Node();
	KD_Node(Tree_Point t_point);
	~KD_Node();

	bool isleaf() {
		return (type == LEAF);
	}
	bool isRoot() {
		return root;
	}


	//getters, setters
	Tree_Point Get_point() { return this->p_tree; }
	KD_Node* Get_leftTree() { return this->left_Tree; };
	KD_Node* Get_rightTree() { return this->right_Tree; };
	int Get_type() { return this->type; }
	int Get_depth() { return this->depth; }
	void set_Type(int T) { this->type = T; }
	void set_Root(bool bool_Root) { this->root = bool_Root; }
	void set_Depth(int depth) { this->depth = depth; }
	void set_Point(Tree_Point t_point) { this->p_tree = t_point; }
	void set_Left(KD_Node* left) { this->left_Tree = left; }
	void set_Right(KD_Node* right) { this->right_Tree = right; }

	//printing
	void print_Info();
	void print_Type();
	void print_Point(Tree_Point p) { this->p1 = p; }


private:
	int type;
	bool root;
	int depth;
	Tree_Point p_tree;
	KD_Node* left_Tree;
	KD_Node* right_Tree;
	Tree_Point p1;
};

class KD_tree
{
public:
	//structing
	KD_tree();
	KD_tree(std::vector<Tree_Point> all_points);
	~KD_tree();

	//helpers
	void initial_tree(std::vector<Tree_Point> all_points);
	KD_Node* KD_Maker(std::vector<Tree_Point> X_point, std::vector<Tree_Point> Y_point, int depth);
	void remaker_KD(std::vector<Tree_Point> all_points);
	int compute_KD_Height();
	void levelOrderPts();
	void add_Level(KD_Node* node, int depth);
	void deallocate_tree(KD_Node* node);

	//check for even points
	bool Even_pts() { return (num_kd_nodes % 2 == 0); }

	//getters
	int get_treeHeight() { return this->height_Tree; }
	int get_num_KDNodes() { return this->num_kd_nodes; }
	KD_Node* get_Root() { return this->root; }
	std::vector<KD_Node*> get_Points() { return this->ordered_pts_set; }

	//setters
	void set_TreeRoot(KD_Node* root) { this->root = root; }
	void set_TreeHeight(int height) { this->height_Tree = height; }
	void init_Height() { this->height_Tree = 1; }
	void set_numKDNodes(int num) { this->num_kd_nodes = num; }
	void set_Pts(std::vector<Tree_Point> all_points) { this->pts_set = all_points; }

	//print
	void printTree();
	void printInfo();
	void printNumNodes();

private:
	KD_Node* root;
	int height_Tree;
	int num_kd_nodes;
	std::vector<Tree_Point> pts_set;
	std::vector<KD_Node*> ordered_pts_set;

	//these structs sort the point based on coordinate
	struct sort_X_coords
	{
		bool operator() (const Tree_Point& P, const Tree_Point& Q)
		{
			if (fabs(P.xpos - Q.xpos) < EPSILON) {
				return(P.ypos < Q.ypos);
			}
			else {
				return (P.xpos < Q.xpos);
			}
		}
	}sort_X_coords;

	struct _sort_Y_coords
	{
		bool operator() (const Tree_Point& P, const Tree_Point& Q)
		{
			if (fabs(P.ypos - Q.ypos) < EPSILON)
			{
				return (P.xpos < Q.xpos);
			}
			else
			{
				return (P.ypos < Q.ypos);
			}

		}
	}sort_Y_coords;
};

KD_Node::KD_Node()
{
	Tree_Point empty_point;
	empty_point.xpos = 0.0f;
	empty_point.ypos = 0.0f;
	set_Point(empty_point);
	set_Type(LEAF);
	set_Root(false);
	set_Depth(0);
	set_Left(NULL);
	set_Right(NULL);
}

KD_Node::KD_Node(Tree_Point t_point)
{
	set_Point(t_point);

	//sets each point at a leaf when the kd tree is built
	set_Type(LEAF);
	set_Root(false);
	set_Depth(1);
	set_Left(NULL);
	set_Right(NULL);
}

void KD_Node::print_Info()
{
	//prints the point data
	print_Point(Get_point());
	//prints the cut type
	print_Type();
	if (!isRoot()) {
		printf("Depth: %d\n", Get_depth());
	}
}

void KD_Node::print_Type()
{
	std::string type;
	if (isRoot()) return; //tree will state if it's printing the root
	if (Get_type() == VERTICAL) {
		type = "VERTICAL";
	}
	else if (Get_type() == HORIZONTAL) {
		type = "HORIZONTAL";
	}
	else if (Get_type() == LEAF) {
		type = "LEAF";
	}
	printf("Type: %s\n", type.c_str());
}

//Node Destructor
KD_Node::~KD_Node() {

}

//default constructor
KD_tree::KD_tree()
{
	set_TreeRoot(NULL);
	set_TreeHeight(0);
	set_numKDNodes(0);
}

//constructor
KD_tree::KD_tree(std::vector<Tree_Point> all_points)
{
	//sort input vector by x-coordinates
	std::sort(all_points.begin(), all_points.end(), sort_X_coords);
	//saves the points as initilaized to class
	set_Pts(all_points);
	set_numKDNodes(all_points.size());
	//set height of KD tree
	init_Height();

	initial_tree(all_points);
}

void KD_tree::remaker_KD(std::vector<Tree_Point> all_points)
{
	//clear vectors and deallocate tree
	pts_set.clear();
	ordered_pts_set.clear();
	deallocate_tree(get_Root());
	set_TreeRoot(NULL);

	//build tree again with same constructor
	std::sort(all_points.begin(), all_points.end(), sort_X_coords);
	set_Pts(all_points);
	set_numKDNodes(all_points.size());
	init_Height();

	initial_tree(all_points);
}

void KD_tree::initial_tree(std::vector<Tree_Point> all_points)
{
	//if there is only one coordinate in all points
	if (num_kd_nodes == 1)
	{
		KD_Node* root = new KD_Node(pts_set[0]);
		root->set_Depth(get_treeHeight());
		root->set_Root(true);
		set_TreeRoot(root);
	}
	else if (num_kd_nodes == 2)
	{
		KD_Node* root = new KD_Node(pts_set[1]);
		root->set_Depth(get_treeHeight() + 1);
		root->set_Type(INITIAL_CUT);
		root->set_Root(true);
		set_TreeRoot(root);

		KD_Node* left_Tree = new KD_Node(pts_set[0]);
		root->set_Depth(get_treeHeight() + 1);
		root->set_Root(true);
		set_TreeRoot(root);

		set_TreeHeight(compute_KD_Height());
	}
	else
	{
		//initialize root node
		int median_index = num_kd_nodes / 2;

		KD_Node* root = new KD_Node(all_points[median_index]);
		root->set_Type(INITIAL_CUT);
		root->set_Depth(get_treeHeight());
		root->set_Root(true);
		set_TreeRoot(root);

		//make vectors for sorted list of positions
		std::vector<Tree_Point> x_left_array, x_right_array, y_left_array, y_right_array;

		//Initialize by copying data from pts_set
		//for loops done seperately because left/right arrays could be different sizes
		//if the number of points is even, the right array is one index
		//smaller than the left
		// Example:
		// 0 1 2 3 4, median index 2 --> left [0,1] right [3,4]
		// 0 1 2 3, median 2 --> left [0,1] right [3]

		for (int i = 0; i < median_index; i++) {
			x_left_array.push_back(pts_set[i]);
			y_left_array.push_back(pts_set[i]);
		}
		for (int i = median_index + 1; i < num_kd_nodes; i++) {
			x_right_array.push_back(pts_set[i]);
			y_right_array.push_back(pts_set[i]);
		}

		std::sort(x_left_array.begin(), x_left_array.end(), sort_X_coords);
		std::sort(x_right_array.begin(), x_right_array.end(), sort_X_coords);
		std::sort(y_left_array.begin(), y_left_array.end(), sort_Y_coords);
		std::sort(y_right_array.begin(), y_right_array.end(), sort_Y_coords);

		//use recursion to build tree
		//use the depth of the root (distance of the root node and its leaf) for
		//recursice call because height is updated during the call
		root->set_Left(KD_Maker(x_left_array, y_left_array, root->Get_depth() + 1));
		root->set_Right(KD_Maker(x_right_array, y_right_array, root->Get_depth() + 1));

		//check
		set_TreeHeight(compute_KD_Height());
	}
	//clear points for a clean start
	//traverse tree and save the points in level order
	pts_set.clear();
	levelOrderPts();
}

/*
Using recursion to build K-D Tree; takes vectors of points sorted by X and Y also the current depth(height)
of the tree and returns a pointer to a node, which is stored as the left or right node of the parent.

Base:
If vector of points only contains one point then this point is a leaf, and the children of this node are NULL.
If vector of points is empty then all conditions will be checked and failed.

If and Else block:
If the vector only has one point (base case)
If the vector has more than one point and the depth is odd, then the cut type is VERTICAL.
If the vector has more than one point and the depth is even, then the cut type is HORIZONTAL.

Parameters:
Vectors of the same n points sorted by their x-coordinates, and sorted by their y-coordinates. The size of these is approximately half (-1 or +1) of the size of the vectors at the previous level.
The value of depth is the height of the AFTER the new nodes have been created, so that the height of the tree does not change if the vector is empty.
*/
KD_Node* KD_tree::KD_Maker(std::vector<Tree_Point> X_point, std::vector<Tree_Point> Y_point, int depth)
{
	int num = X_point.size();
	int middle_position = num / 2;

	//Base Case: if only one point, return leaf containing point If the height of the tree if ODD at the new node, then the cut type is VERTICAL,
	//The root the height is 1 and the initial cut is VERTICAL,
	//Thus if the height is even then the cut type is HORIZONTAL 

	if (num == 1)
	{
		KD_Node* node = new KD_Node(X_point[0]);
		//default type is LEAF
		node->set_Depth(depth);
		//a check
		node->set_Left(NULL);
		node->set_Right(NULL);

		set_TreeHeight(depth);
		return node;
	}
	//Height is odd at new node, with cut type VERTICAL
	else if (depth % 2 != 0 && num > 1)
	{
		//Create new node and add median point to the three
		KD_Node* odd_node = new KD_Node(X_point[middle_position]);
		odd_node->set_Type(VERTICAL);
		odd_node->set_Depth(depth);

		//Split x-coordinates into two parts and sort the parts by y-coordinates then next cut is HORIZONTAL
		std::vector<Tree_Point> x_left_odd, x_right_odd, y_left_odd, y_right_odd;

		//copy data
		for (int i = 0; i < middle_position; i++)
		{
			x_left_odd.push_back(X_point[i]);
			y_left_odd.push_back(X_point[i]);
		}
		for (int i = middle_position + 1; i < num; i++)
		{
			x_right_odd.push_back(X_point[i]);
			y_right_odd.push_back(X_point[i]);
		}

		//sort y coordinates
		std::sort(y_left_odd.begin(), y_left_odd.end(), sort_Y_coords);
		std::sort(y_right_odd.begin(), y_right_odd.end(), sort_Y_coords);

		//Recursive calls
		odd_node->set_Left(KD_Maker(x_left_odd, y_left_odd, depth + 1));
		odd_node->set_Right(KD_Maker(x_right_odd, y_right_odd, depth + 1));

		set_TreeHeight(depth);
		return odd_node;
	}
	//Depth is even and num > 1, cut is HORIZONTAL
	else if (depth % 2 == 0 && num > 1)
	{
		//add node to the tree
		KD_Node* even_node = new KD_Node(Y_point[middle_position]);
		even_node->set_Type(HORIZONTAL);
		even_node->set_Depth(depth);

		//Split x coordinates into two parts and sort the parts by y coordinates.
		//LEFT refers to the part above the horizontal line, RIGHT refers to the points below it
		std::vector<Tree_Point> x_left_even, x_right_even, y_left_even, y_right_even;
		x_right_even;
		for (int i = 0; i < middle_position; i++)
		{
			x_left_even.push_back(Y_point[i]);
			y_left_even.push_back(Y_point[i]);
		}
		for (int i = middle_position + 1; i < num; i++)
		{
			x_right_even.push_back(Y_point[i]);
			y_right_even.push_back(Y_point[i]);
		}

		//sort x coordinates
		std::sort(x_left_even.begin(), x_left_even.end(), sort_X_coords);
		std::sort(x_right_even.begin(), x_right_even.end(), sort_X_coords);

		//recursive calls
		even_node->set_Left(KD_Maker(x_left_even, y_left_even, depth + 1));
		even_node->set_Right(KD_Maker(x_right_even, y_right_even, depth + 1));

		set_TreeHeight(depth);
		return even_node;
	}
	//ELSE
	return NULL;
}

/* Returns an int whose value is the height of the tree.
For checking correctness: HEIGHT = CEIL ( lg(NUM_NODES) )
traverses tree from root to leftmost leaf, increments a counter, and returns that value
If n is even, then the "left side" array is larger than the right – thus,
the longest path from root to leaf is the leftmost one.
Height is computed on the fly; this method was originally written to make sure that those
values were correct. It is redundant to leave it in, but it serves as a sanity check.
*/
int KD_tree::compute_KD_Height()
{
	KD_Node* temp_node = root;
	int height = 1;

	while (!temp_node->isleaf()) {
		height++;
		temp_node = temp_node->Get_leftTree();
	}
	return height;
}

/* LEVEL ORDER POINTS
adds all nodes in KD-Tree to the pts vector using a simple
breadth first traversal
Iterates through the "levels" of the tree – calls addLevel to traverse
tree to find appropriate nodes.
*/
void KD_tree::levelOrderPts()
{
	for (int depth = 1; depth < height_Tree + 1; depth++) {
		add_Level(get_Root(), depth);
	}
}

/* ADD LEVEL adds all nodes at depth x to the vector, from left->right
called by levelOrderPts()
BASE CASES: node is NULL (parent only has one child node, or invalid input);
node is at desired depth, so add to the vector and exit
RECURSIVE CALLS: traverse subtrees and print all nodes of desired depth
*/
void KD_tree::add_Level(KD_Node* tree_node, int depth)
{
	if (!tree_node)
	{
		return;
	}
	if (tree_node->Get_depth() == depth)
	{
		ordered_pts_set.push_back(tree_node);
		return;
	}
	add_Level(tree_node->Get_leftTree(), depth);
	add_Level(tree_node->Get_rightTree(), depth);
}

void KD_tree::printInfo()
{
	printf("TREE INFO\n");
	printNumNodes();
	printf("Height %d\n", get_treeHeight());
	printf("Root: ");
}

//prints nodes in tree
void KD_tree::printTree()
{
	if (!get_Root())
	{
		printf("Root is NULL; tree is empty.\n");
	}
	printf("\n\PRINTING TREE:\n");
	for (int i = 0; i < ordered_pts_set.size(); i++)
	{
		ordered_pts_set[i]->print_Info();
		printf("\n");
	}
}

void KD_tree::printNumNodes() {
	printf("Number of nodes %d\n", get_num_KDNodes());
}

/* Helper function for the destructor
Recurses through entire tree and call Node destructor to deallocate
(delete) every node in the tree.
*/
void KD_tree::deallocate_tree(KD_Node* tree_node)
{
	if (tree_node->isleaf() || !tree_node) {
		return;
	}
	deallocate_tree(tree_node->Get_leftTree());
	deallocate_tree(tree_node->Get_rightTree());
	delete(tree_node);
}

/* DESTRUCTOR
delete nodes from leaves to the root's left/right children,
then delete root
*/
KD_tree::~KD_tree()
{
	deallocate_tree(get_Root());
}


#endif _KDTREENODEMAKER_H