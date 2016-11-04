#pragma once

#include <stdint.h>
#include <assert.h>
#include "Utility.h"

template<typename TNode>
class RedBlackTreeInOrderIterator {
	TNode *current;

protected:
	inline typename TNode::ValueTy &dereference() const {
		return current->value;
	}

	void moveNext() {
		if (!current) {
			//End of iterator
			return;
		}

		if (current->right) {
			current = current->right;
			while (current->left) {
				current = current->left;
			}
		} else {
			TNode *parent;
			while (parent = current->parent) {
				if (parent->left == current)
					break;

				current = parent;
			}

			current = parent;
		}
	}

public:
	RedBlackTreeInOrderIterator(TNode *node) {
		current = node;
	}

	//Dereference
	inline typename TNode::ValueTy &operator*() const {
		return dereference();
	}

	inline typename TNode::ValueTy *operator->() const {
		return &dereference();
	}

	//Prefix inc
	inline RedBlackTreeInOrderIterator &operator++() {
		moveNext();
		return *this;
	}

	//Postfix inc
	inline RedBlackTreeInOrderIterator operator++(int) {
		TNode *old = current;
		moveNext();
		return RedBlackTreeInOrderIterator(old);
	}
	
	//Can't rewind, no dec operators.

	//Equality & inequality.
	inline bool operator==(RedBlackTreeInOrderIterator b) const {
		return current == b.current;
	}

	inline bool operator!=(RedBlackTreeInOrderIterator b) const {
		return current != b.current;
	}
};

template<typename TValue, typename TKey, typename Comparator = Less<TKey>>
class RedBlackTree {
	Comparator compare;

	struct Node {
		typedef typename TValue ValueTy;
		typedef typename TKey KeyTy;

		Node *left, *right, *parent;
		bool red;
		TValue value;
		TKey key;

		Node(TKey key, TValue value) : key(key), value(value) {
			parent = left = right = nullptr;
			red = true;
		}
		~Node() {
			//FIXME: Delete from bottom up to prevent recursion.
			delete left;
			delete right;
		}

		inline bool isRed() const {
			//return (parent & 0x01) == 1;
			return red;
		}

		inline void setRed(bool red) {
			this->red = red;
			//parent = (Node*)((((intptr_t)parent) & ~((intptr_t)0x00000001)) | ((intptr_t)(red ? 0x01 : 0x00)));
		}

		inline Node *getParent() {
			return parent;
		}
		inline const Node *getParent() const {
			return parent;
		}
		inline void setParent(Node *node) {
			parent = node;
		}
	};

	Node *root;

	Node *createNode(TKey key, TValue value) {
		return new Node(key, value);
	}

	//To preserve node's children, they must have been set to nullptr.
	void destroyNode(Node *node) {
		delete node;
	}

	Node *nodeContaining(TKey key) const  {
		Node *node = root;
		while (node) {
			if (compare(node->key, key)) {
				node = node->right;
			} else if (compare(key, node->key)) {
				node = node->left;
			} else {
				return node;
			}
		}

		//There is no node containing key.
		return nullptr;
	}

	void rebalance(Node *node) {
		while (true) {
			Node *parent = node->parent;

			if (parent == nullptr) {
				//Case 1
				node->setRed(false);
				break;
			}

			if (!parent->isRed()) {
				//Case 2
				break;
			}

			//Parent is red, therefore cannot be root.
			//Therefore grandparent cannot be null.
			Node *grandparent = parent->parent;
			assert(grandparent && "Somehow a red node became root.");
			Node *uncle = grandparent->left == parent ? grandparent->right : grandparent->left;

			if (uncle != nullptr && uncle->isRed()) {
				//Case 3
				parent->setRed(false);
				uncle->setRed(false);
				grandparent->setRed(true);
				node = grandparent;
				continue;
			}

			//Case 4
			if (node == parent->right && parent == grandparent->left) {
				//Rotate Left
				grandparent->left = node;	//Replace parent as g's left.
				parent->right = node->left;	//Replace ourselves in parent with our left.
				node->left = parent;		//Put parent as our left.

				//Fix parent pointers.
				parent->parent = node;
				node->parent = grandparent;
				if (parent->right)
					parent->right->parent = parent;

				node = parent;
				parent = node->parent;
			} else if (node == parent->left && parent == grandparent->right) {
				//Rotate right
				grandparent->right = node;	//Replace parent as g's right.
				parent->left = node->right;	//Replace ourselves in parent with our right.
				node->right = parent;		//Put parent as our right.

				//Fix parent pointers
				parent->parent = node;
				node->parent = grandparent;
				if (parent->left)
					parent->left->parent = parent;

				node = parent;
				parent = node->parent;
			}

			//Case 5
			parent->setRed(false);
			grandparent->setRed(true);

			if (grandparent->parent == nullptr) {
				root = parent;
			} else {
				if (grandparent->parent->left == grandparent)
					grandparent->parent->left = parent;
				else
					grandparent->parent->right = parent;
			}
			parent->parent = grandparent->parent;
			if (node == parent->left) {
				//Rotate right
				//Fix grandparent
				grandparent->left = parent->right;
				if (grandparent->left)
					grandparent->left->parent = grandparent;

				//Move grandparent
				parent->right = grandparent;
			} else {
				//Rotate left
				//Fix grandparent
				grandparent->right = parent->left;
				if (grandparent->right)
					grandparent->right->parent = grandparent;

				//Move grandparent
				parent->left = grandparent;
			}
			grandparent->parent = parent;
			break;
		}
	}

public:
	typedef typename TValue ValueTy;
	typedef typename TKey KeyTy;
	typedef typename RedBlackTreeInOrderIterator<Node> IteratorTy;

	RedBlackTree() : root(nullptr) {

	}

	~RedBlackTree() {
		delete root;
	}

	inline bool tryGet(TKey key, TValue &value) const {
		Node *node = nodeContaining(key);
		if (node) {
			value = node->value;
			return true;
		} else {
			return false;
		}
	}

	inline bool contains(TKey key) const {
		return nodeContaining(key) != nullptr;
	}

	inline TValue get(TKey key) const {
		Node *node = nodeContaining(key);
		assert(node && "get called when key not present!");
		return node->value;
	}

	void set(TKey key, TValue value) {
		Node *node = nodeContaining(key);
		assert(node && "set called when key not present!");
		node->value = value;
	}

	void insert(TKey key, TValue value) {
		//If we have no root, things are quite easy.
		if (root == nullptr) {
			root = createNode(key, value);
			root->setRed(false);
			return;
		}

		Node *newNode = createNode(key, value);
		//Nodes are red by default.

		Node *current = root;
		while (true) {
			if (compare(current->key, key)) {
				//Move right
				if (current->right) {
					current = current->right;
				} else {
					current->right = newNode;
					newNode->parent = current;
					break;
				}
			} else if (compare(key, current->key)) {
				//Move left
				if (current->left) {
					current = current->left;
				} else {
					current->left = newNode;
					newNode->parent = current;
					break;
				}
			} else {
				//key already exists.
				current->value = value;
				destroyNode(newNode);
				return;
			}
		}

		rebalance(newNode);
	}

	void remove(TKey key) {
		Node *node = nodeContaining(key);
		assert(node && "remove called when key not present!");

		//TODO: RedBlackTree::remove
	}

	inline TValue minimum() const {
		assert(root && "Minimum called on empty tree.");
		Node *node = root;
		while (node->left)
			node = node->left;
		return node->value;
	}

	inline TValue maximum() const  {
		assert(root && "Maximum called on empty tree.");
		Node *node = root;
		while (node->right)
			node = node->right;
		return node->value;
	}

	inline IteratorTy begin() {
		if (!root)
			return end();

		Node *least = root;
		while (least->left)
			least = least->left;

		return IteratorTy(least);
	}

	inline IteratorTy end() {
		return IteratorTy(nullptr);
	}
};

#include "RedBlackTree.tpp"
