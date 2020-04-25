class Node
{
  public:
    void *ptr;
    Node *next;
};

class LinkedList
{
private:
    Node *head;
    Node *tail;
public:
    int length = 0;

    LinkedList()
    {
      length = 0;
      head = NULL;
      tail = NULL;
    }

    Node* get_node(unsigned int pos)
    {
      Node* current = head;
      if (pos > 0) {
        for (int i = 0; i < pos; i++) {
          current = current->next;
        }
      }

      return current;
    }

    int get_length()
    {
      return length;
    }

    void add_node(void *ptr)
    {
        auto *tmp = new Node();
        tmp->ptr = ptr;
        tmp->next = NULL;

        if (head == NULL)
        {
            head = tmp;
            tail = tmp;
        }
        else
        {
            tail->next = tmp;
            tail = tail->next;
        }

        length++;
    }

    void remove_node(unsigned int pos)
    {
      if (length > 0 && pos < length) {
        Node* tmp = get_node(pos);
        if (length == 1) {
          head = NULL;
          tail = NULL;
        }
        else {
          if (pos == 0) {
            head = tmp->next;
          }
          else {
            Node* before = get_node(pos - 1);
            before->next = tmp->next;
            if (pos == length - 1) {
              tail = before;
            }
          }
        }

        delete tmp;
        length--;
      }
    }
};