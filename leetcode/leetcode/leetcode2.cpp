#include <iostream>

using namespace std;

struct ListNode {
	int val;
	ListNode* next;

	ListNode() : val(0), next(nullptr) {}
	ListNode(int x) : val(x), next(nullptr) {}
	ListNode(int x, ListNode* next) : val(x), next(next) {}
};

ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
    if (!l1 || !l2) {
        return nullptr;
    }

    ListNode* pre = nullptr;
    auto p1 = l1;
    auto p2 = l2;
    auto p3 = new ListNode;
    auto head = p3;
    int plus = 0;
    while (p1 && p2) {
        auto sum = p1->val + p2->val + plus;
        plus = 0;
        if (sum >= 10) {
            plus = sum / 10;
            sum %= 10;
        }
        p3->val = sum;
        p3->next = new ListNode;
        pre = p3;
        p3 = p3->next;
        p1 = p1->next;
        p2 = p2->next;
    }

    if (p1) {
        while (p1) {
            auto sum = p1->val + plus;
            plus = 0;
            if (sum >= 10) {
                plus = sum / 10;
                sum %= 10;
            }
            p3->val = sum;
            p3->next = new ListNode;
            pre = p3;
            p3 = p3->next;
            p1 = p1->next;
        }
    }
    else if (p2) {
        while (p2) {
            auto sum = p2->val + plus;
            plus = 0;
            if (sum >= 10) {
                plus = sum / 10;
                sum %= 10;
            }
            p3->val = sum;
            p3->next = new ListNode;
            pre = p3;
            p3 = p3->next;
            p2 = p2->next;
        }
    }

    if (plus > 0) {
        p3->val = plus;
    }
    else {
        delete p3;
        pre->next = nullptr;
    }
    return head;
}

int main() {
    auto p1 = new ListNode(2);
    auto l1 = p1;
    p1->next = new ListNode(4);
    p1->next->next = new ListNode(3);

    auto p2 = new ListNode(5);
    auto l2 = p2;
    p2->next = new ListNode(6);
    p2->next->next = new ListNode(4);

    auto head = addTwoNumbers(l1, l2);
    while (head) {
        cout << head->val << " ";
        auto tmp = head;
        head = head->next;
        delete tmp;
    }
}