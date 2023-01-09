#pragma once
#include "Node.h"

class List
{
	public:
		List();
		virtual ~List();
		void insertAfterTargetNode(Node*& nodeToInsert, Node*& targetNode);
		void insertBeforeTargetNode(Node*& nodeToInsert, Node*& targetNode);
		void addNodeBeforeTail(Node*& node);

		Node* popNode(Node* node);
		Node*& getTail();
		Node*& getHead();

		bool isListEmpty();
		int getLen();
		void setLen(int len);
		int getSize();
		virtual void flushList(PacketList& packetList);

		void setName(const char* name);

	private:
		Node* head;
		Node* tail;
		int len;
		int size;
		char* name;
};