#include <iostream>
#include <string>
#include <vector>
#include <set>

using namespace std;

#define ALPHABET_SIZE 26
#define MAX_STATES 100

int _goto[MAX_STATES][ALPHABET_SIZE];
int _fail[MAX_STATES];
set<string> _out[MAX_STATES];   // 使用set是因为在生成fail表，同时更新out表过程中，有可能向同一位置多次插入同一个模式串


inline void buildGoto(const vector<string>& patterns)
{
    unsigned int used_states;
    unsigned int t;

    vector<string>::const_iterator vit; // 如果传入const vector,就必须使用const_iterator来迭代
    string::const_iterator sit;

    for(vit = patterns.begin(), used_states = 0; vit != patterns.end(); ++vit) {
        for(sit = vit->begin(), t = 0; sit != vit->end(); ++sit) {
            if(_goto[t][(*sit)-'a'] == 0) {
                _goto[t][(*sit)-'a'] = ++used_states;
                t = used_states;
            }
            else {
                t = _goto[t][(*sit)-'a'];
            }
        }

        _out[t].insert(*vit);
    }
}

inline void buildFail(const vector<string>& patterns)
{
    unsigned int t, m, last_state;
    unsigned int s[20];

    vector<string>::const_iterator vit;
    string::const_iterator sit1, sit2, sit3;

    for(vit = patterns.begin(); vit != patterns.end(); ++vit) {
        // 先要找到输入的单词的各字母对应的状态转移表的状态号，由于状态转移表没有
        // 记录各状态的前驱状态信息，该步暂时无法掠过
        t = 0;
        m = 0;
        sit1 = vit->begin();

        while(sit1 != vit->end() && _goto[t][*sit1-'a'] != 0) {
            t = _goto[t][*sit1-'a'];
            ++sit1;
            s[m++] = t;
        }

        for(sit1 = vit->begin() + 1; sit1 != vit->end(); ++sit1) {
            // 此时的[sit2, sit1+1)就是当前模式的一个非前缀子串
            for(sit2 = vit->begin() + 1; sit2 != sit1 + 1; ++sit2) {
                t = 0;
                sit3 = sit2;

                // 对该子串在goto表中执行状态转移
                while(sit3 != sit1 + 1 && _goto[t][*sit3-'a'] != 0) {
                    t = _goto[t][*sit3-'a'];
                    ++sit3;
                }

                // 当前子串可以使goto表转移到一个非0位置
                if(sit3 == sit1 + 1) {
                    // 求出输入当前子串在goto表中所转移到的位置
                    last_state = s[sit3 - vit->begin() - 1];

                    // 更新该位置的fail值，如果改为值的fail值为0，则用t值替换，因为对于sit1而言，
                    // 是按照以其为末尾元素的非前缀
                    // 子串的由长到短的顺序在goto表中寻找非0状态转移的，而满足条件的t是这里面的最长子串
                    if(_fail[last_state] == 0) {
                        _fail[last_state] = t;
                    }

                    // 如果两者都标识完整的模式串
                    if(_out[last_state].size() > 0 && _out[t].size() > 0) {
                        // 将out[t]内的模式串全部加入out[last_state]中
                        for(set<string>::const_iterator cit = _out[t].begin(); cit != _out[t].end(); ++cit) {
                            _out[last_state].insert(*cit);
                        }
                    }
                }
            }
        }
    }
}


void AC(const string& text, const vector<string>& patterns)
{
    unsigned int t = 0;
    string::const_iterator sit = text.begin();

    buildGoto(patterns);
    buildFail(patterns);

    // 每次循环中， t都是*sit的前置状态
    while(sit != text.end()) {
        // 检查是否发现了匹配模式，如果有，将匹配结果输出
        if(_out[t].size() > 0) {
            cout << (sit - text.begin() - 1) << ": ";

            for(set<string>::const_iterator cit = _out[t].begin(); cit != _out[t].end(); ++cit) {
                cout << (*cit) << ", ";
            }

            cout << '\n';
        }
        if(_goto[t][*sit-'a'] == 0) {
            t = _fail[t];

            // 找到可以实现非0跳转的fail状态转移
            while(t != 0 && _goto[t][*sit-'a'] == 0) {
                t = _fail[t];
            }
            if(t == 0) {
                // 跳过那些在初始状态不能实现非0状态跳转的字母输入
                if(_goto[0][*sit-'a'] == 0) {
                    ++sit;
                }
                continue;
            }
        }

        t = _goto[t][*sit-'a'];
        ++sit;
    }
}

int main()
{
    const string str = "ushers";
    vector<string> patterns;
    patterns.push_back("she");
    patterns.push_back("he");
    patterns.push_back("her");
    patterns.push_back("hers");
    AC(str, patterns);
    return 0;
}
