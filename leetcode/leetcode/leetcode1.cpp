#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <map>

using namespace std;
#if 0
/* ����ѭ�� O(n2) */
vector<int> twoSum(vector<int>& nums, int target) {
	const auto size = nums.size();
	int i = 0, j = 0;
	for (; i < size - 1; ++i) {
		for (j = i + 1; j < size; ++j) {
			if (nums[i] + nums[j] == target) {
				return { i, j };
			}
		}
	}
	return {};
}
#endif

/* ʹ�ÿռ任ȡʱ�� */
vector<int> twoSum(vector<int>& nums, int target) {
	// ��map������Ϊmultimap��key�ǿ����ظ���
	multimap<int, int> imap;
	for (int i = 0; i != nums.size(); ++i) {
		int delta = target - nums[i];
		auto it = imap.find(delta);
		if (it != imap.end()) {
			return vector<int>({ it->second , i });
		}
		imap.insert({ nums[i], i });
	}
	return vector<int>();
}

int main() {
	vector<int> test{ 3, 2, 4 };
	auto res = twoSum(test, 6);
	for (auto& val : res) {
		cout << val << " ";
	}

}