#include "PacketList.h"
#include "List.h"
#include "iostream"
#include "messagePipe.h"
#include <cstring>

using namespace std;

List::List()
{
    head = new Node();
    tail = new Node();

    head->next = tail;
    tail->prev = head;

    len = 0;
    size = 0;
    name = nullptr;
}

List::~List()
{
    //std::stringstream messageStream;
    //messageStream << "Destructing " << name << " list...";
    //std::string finalMessage = messageStream.str();
    //sendMessage(finalMessage.c_str());

    //cout << "you should execute after sendMessageEx function" << endl;

    //cout << "List destructor was called" << endl;
    Node* current;
    while (head != nullptr)
    {
        current = head;
        head = head->next;
        delete current;
    }

    //messageStream.str("");
    //messageStream << name << " list destroyed.";
    //sendMessageEx(messageStream);

    if (name != nullptr) delete[] name;
}

void List::setName(const char* name)
{
    if (this->name != nullptr) delete[] this->name;
    this->name = new char[strlen(name) + 1];
    strcpy_s(this->name, strlen(name) + 1, name);
}

void List::insertAfterTargetNode(Node*& node, Node*& target)
{
    if (node == nullptr || node == head || node == tail || target == tail)
    {
        /*if (node == nullptr) cerr << "Cannot insert invalid node" << endl;
        else if (node == head) cerr << "Cannot insert first node" << endl;
        else if (node == tail) cerr << "Cannot insert last node" << endl;
        else if (target == tail) cerr << "Cannot insert after last node" << endl;
        return;*/

        if (node == nullptr) sendMessage("Cannot insert invalid node.");
        else if (node == head) sendMessage("Cannot insert first node.");
        else if (node == tail) sendMessage("Cannot insert last node.");
        else if (target == tail) sendMessage("Cannot insert after last node");
        return;
    }

    Node* nextNode = target->next;

    target->next = node;
    nextNode->prev = node;

    node->next = nextNode;
    node->prev = target;

    size++;
}

void List::insertBeforeTargetNode(Node*& node, Node*& target)
{
    if (node == nullptr || node == head || node == tail || target == head)
    {
        /*if (node == nullptr) cerr << "Cannot insert invalid node" << endl;
        else if (node == head) cerr << "Cannot insert first node" << endl;
        else if (node == tail) cerr << "Cannot insert last node" << endl;
        else if (target == head) cerr << "Cannot insert before first node" << endl;
        return;*/

        if (node == nullptr) sendMessage("Cannot insert invalid node.");
        else if (node == head) sendMessage("Cannot insert first node.");
        else if (node == tail) sendMessage("Cannot insert last node.");
        else if (target == head) sendMessage("Cannot insert before first node.");
        return;
    }

    Node* prev_node = target->prev;
    prev_node->next = node;
    target->prev = node;

    node->prev = prev_node;
    node->next = target;

    size++;
}


Node* List::popNode(Node* node)
{
    if (node == head)
    {
        sendMessage("Cannot pop first node.");
        //cerr << "Cannot pop first node" << endl;
        exit(1);
    }
    if (node == tail)
    {
        sendMessage("Cannot pop last node.");
        //cerr << "Cannot pop last node" << endl;
        exit(1);
    }

    Node* next = node->next;
    Node* prev = node->prev;


    prev->next = next;
    next->prev = prev;

    size--;

    return node;
}

Node*& List::getTail()
{
    return this->tail;
}

Node*& List::getHead()
{
    return this->head;
}

void List::addNodeBeforeTail(Node*& node)
{
    insertBeforeTargetNode(node, tail);
}

bool List::isListEmpty()
{
    return head->next == tail;
}

int List::getLen()
{
    return this->len;
}

void List::setLen(int len)
{
    this->len = len;
}

void List::flushList(PacketList& packetList)
{
    Node* prev = packetList.getTail()->prev;

    // flush all buffered packets
    while (!isListEmpty())
    {
        Node* current = packetList.popNode(getTail()->prev);
        packetList.insertAfterTargetNode(current, prev);
        setLen(getLen() - 1);
    }
}

int List::getSize()
{
    return this->size;
}