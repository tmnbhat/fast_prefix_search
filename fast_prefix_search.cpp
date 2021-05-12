// Implementation of data structure defined in:
// "Fast Prefix Search in Little Space, with Applications" by Djamal Et Al.

#include <iostream>
#include <string>
#include <math.h>
#include <unordered_map>
#include <vector>
#include <bitset>
#include <algorithm>

using namespace std;

// uncomment this to enable logs
//#define ENABLE_LOGS

std::unordered_map <std::string, int> F; // Active use
std::unordered_map <std::string, int> G; // Active use
vector<std::string> P;
string b = ""; // Active use
int s = 1, n = 0, l = 0;
std::unordered_map <std::string, bool> temp_is_name_of_leaf;
std::unordered_map <std::string, int> P_to_index;   // Active use (to be replaced with Monotone Minimal Perfect Hashing function)

// Compressed trie node
struct TrieNode
{
    struct TrieNode *zeroChild;
    struct TrieNode *oneChild;

    string node;
};
  
// Returns new trie node (initialized to NULLs)
struct TrieNode *getNode(void)
{
    struct TrieNode *pNode =  new TrieNode;
  
    pNode->zeroChild = NULL;
    pNode->oneChild = NULL;
    pNode->node="";
  
    return pNode;
}

// If not present, inserts key into trie
void insertCompressed(struct TrieNode *root, string key)
{
    struct TrieNode *pCrawl = root;
  
    for (int i = 0; i < key.length(); i++)
    {
        if (pCrawl->node != "")
        {
            for (int j = 0; j < pCrawl->node.length() && i < key.length(); j++)
            {
                if(key[i] == pCrawl->node[j])
                    i++;
                else
                {
                    struct TrieNode *pNodeSplit =  getNode();
                        
                    pNodeSplit->zeroChild = pCrawl->zeroChild;
                    pNodeSplit->oneChild = pCrawl->oneChild;
                    pNodeSplit->node = pCrawl->node.substr(j+1, pCrawl->node.length()-(j+1));

                    struct TrieNode *pNodeNew =  getNode();
                    
                    pNodeNew->node = key.substr(i+1, key.length()-(i+1));
                    if('0' == pCrawl->node[j])
                    {
                        pCrawl->zeroChild = pNodeSplit;
                        pCrawl->oneChild = pNodeNew;
                    }
                    else
                    {
                        pCrawl->zeroChild = pNodeNew;
                        pCrawl->oneChild = pNodeSplit;
                    }
                   #ifdef ENABLE_LOGS
                    std::cout << "new node added " << pNodeNew->node << endl;
                    std::cout << "split node " << pCrawl->node;
                    std::cout << " into " << pCrawl->node.substr(0, j);
                    std::cout << " and " << pNodeSplit->node << endl;
                   #endif
                    pCrawl->node = pCrawl->node.substr(0, j);

                    return;
                }
            }
        }
        if('0' == key[i])
        {
            if(pCrawl->zeroChild != NULL)
                pCrawl = pCrawl->zeroChild;
            else
            {
                pCrawl->node = key.substr(i, key.length()-i);
               #ifdef ENABLE_LOGS
                std::cout << "new node added " << pCrawl->node << endl;
               #endif
                return;
            }
        }
        else
        {
            if(pCrawl->oneChild != NULL)
                pCrawl = pCrawl->oneChild;
            else
            {
                pCrawl->node = key.substr(i, key.length()-i);
               #ifdef ENABLE_LOGS
                std::cout << "new node added " << pCrawl->node << endl;
               #endif
                return;
            }
        }
    }
}

// Returns true if key is present in the compressed trie if prefixSearch is set to false, else
// false
// If prefixSearch is set to true, returns true also when the key
// is a prefix of any element in the compressed trie
bool searchCompressed(struct TrieNode *root, string key, bool prefixSearch)
{
    struct TrieNode *pCrawl = root;
    int nodeIndex = 0, keyIndex = 0; 
  
    for (; keyIndex < key.length(); keyIndex++)
    {
        if (pCrawl->node != "")
        {
            
            for (nodeIndex = 0; nodeIndex < pCrawl->node.length() && keyIndex < key.length(); nodeIndex++)
            {
                if(key[keyIndex] == pCrawl->node[nodeIndex])
                    keyIndex++;
                else
                    return false;
            }
        }

        if(keyIndex < key.length())
        {
            if('0' == key[keyIndex])
            {
                if(pCrawl->zeroChild != NULL)
                    pCrawl = pCrawl->zeroChild;
                else
                    return false;
            }
            else
            {
                if(pCrawl->oneChild != NULL)
                    pCrawl = pCrawl->oneChild;
                else
                    return false;
            }
        }
    }

    if(false == prefixSearch)
    {
        if((NULL == pCrawl->zeroChild) && (NULL == pCrawl->oneChild) &&
           (nodeIndex >= pCrawl->node.length()))
            return true;
        else
            return false;
    }
    
    return true;
}

// Prints the nodes present in the compressed trie
void printCompressedTrieNodes(TrieNode *root)
{
    struct TrieNode *pCrawl = root;

    std::cout << "node = " << pCrawl->node << endl;
    if (pCrawl->zeroChild != NULL)
    {
        printCompressedTrieNodes(pCrawl->zeroChild); 
    }
    pCrawl = root;
    if (pCrawl->zeroChild != NULL)
    {
        printCompressedTrieNodes(pCrawl->oneChild); 
    }
}

// Recursive part of the function buildParam
void buildParam_recur(struct TrieNode *pCrawl, string prefixSoFar)
{
    string key = prefixSoFar;
    int prefixSoFarLength, nodeLength;
    int64_t extent1Val;

    prefixSoFarLength = prefixSoFar.length();
    nodeLength = pCrawl->node.length();

    G[prefixSoFar] = prefixSoFarLength + nodeLength; /* param G insert */

   #ifdef ENABLE_LOGS
    std::cout << "G mapped " << prefixSoFar;
    std::cout << " to " << G[prefixSoFar] << endl;
   #endif
    if((NULL == pCrawl->zeroChild) && (NULL == pCrawl->oneChild))
        temp_is_name_of_leaf[prefixSoFar] = true;
    if(pCrawl->node != "")
    {
        int k = (prefixSoFarLength-1)/s;
        
        if(k*s <= prefixSoFarLength-1)
            k++;
        
        int ksNodeIndex = k*s-prefixSoFarLength+1;
        F[key] = prefixSoFarLength;
       #ifdef ENABLE_LOGS
        std::cout << "F mapped " << key;
        std::cout << " to " << F[key] << endl;
       #endif
        for(int i = 0; i < nodeLength; i++)
        {
            key += (pCrawl->node[i]);
            if(std::min(ksNodeIndex, nodeLength))
            {
                F[key] = prefixSoFarLength;
               #ifdef ENABLE_LOGS
                std::cout << "F mapped " << key;
                std::cout << " to " << F[key] << endl;
               #endif
            }
            else if(0 == key.length()%s)
            {
                F[key] = prefixSoFarLength;
               #ifdef ENABLE_LOGS
                std::cout << "F mapped " << key;
                std::cout << " to " << F[key] << endl;
               #endif
            }
        }

        /* start of param P insert*/
        if((NULL != pCrawl->zeroChild) && (NULL != pCrawl->oneChild))
        {
            string extent = prefixSoFar+pCrawl->node;
            string extent1 = extent + "1";

            P.push_back(extent+"0");
            P.push_back(extent1);
            extent1Val = std::stoi(extent1, nullptr, 2);
            int64_t temp = extent1Val;
            while (temp > 0)
            {
                // if the last bit is not set
                if ((temp & 1) == 0)
                {
                    std::string extent1PlusOneString = std::bitset< 64 >(extent1Val+1).to_string();
                    int start_index = extent1PlusOneString.length()-extent1.length();
                    P.push_back(extent1PlusOneString.substr(start_index, extent1.length()));
                    break;
                }
                temp = temp >> 1;
            }
        }
        /* end of param P insert*/
        
    }
    else if((NULL == pCrawl->zeroChild) && (NULL == pCrawl->oneChild))
    {
        F[prefixSoFar] = prefixSoFarLength;
       #ifdef ENABLE_LOGS
        std::cout << "F mapped " << prefixSoFar;
        std::cout << " to " << F[prefixSoFar] << endl;
       #endif
    }
    
    if(pCrawl->zeroChild != NULL)
    {
        buildParam_recur(pCrawl->zeroChild, prefixSoFar+pCrawl->node+"0");
    }
    if(pCrawl->oneChild != NULL)
    {
        buildParam_recur(pCrawl->oneChild, prefixSoFar+pCrawl->node+"1");
    }
}

// Build function maps "F" and "G" and string set "P" described in Section 3.3 of 
// "Fast Prefix Search in Little Space, with Applications" by Djamal Et Al.
//NOTE: needs global variable "s" is be calculated beforehand 
void buildParam(struct TrieNode *root)
{
    struct TrieNode *pCrawl = root;
    int nodeLength = pCrawl->node.length();
    string prefixSoFar = pCrawl->node;
    int64_t extent1Val;

    if(nodeLength != 0)
    {
        for(int i = 0; i < std::min(s, nodeLength); i++)
        {
            F[pCrawl->node.substr(0,i+1)] = 0;
           #ifdef ENABLE_LOGS
            std::cout << "F mapped " << pCrawl->node.substr(0,i+1);
            std::cout << " to 0" << endl;
           #endif
        }
        
        string extent = prefixSoFar;
        string extent1 = extent+"1";

        P.push_back(extent+"0");
        P.push_back(extent1);
        
        /* start of param P insert*/
        extent1Val = std::stoi(extent1, nullptr, 2);
        int64_t temp = extent1Val;
        while (temp > 0)
        {
            // if the last bit is not set
            if ((temp & 1) == 0)
            {
                std::string extent1PlusOneString = std::bitset< 64 >(extent1Val+1).to_string();

                int start_index = extent1PlusOneString.length()-extent1.length();
                P.push_back(extent1PlusOneString.substr(start_index, extent1.length()));
                break;
            }
            temp = temp >> 1;
        }
        /* end of param P insert */
    }

    G[""] = nodeLength; /* param G insert */

   #ifdef ENABLE_LOGS
    std::cout << "G mapped \"\" ";
    std::cout << " to " << nodeLength << endl;
   #endif

    if(root->zeroChild != NULL)
    {
        buildParam_recur(root->zeroChild, prefixSoFar+"0");
    }
    if(root->oneChild != NULL)
    {
        buildParam_recur(root->oneChild, prefixSoFar+"1");
    }

    std::sort(P.begin(), P.end());

    for(int i = 0; i < P.size(); i++)
    {
        P_to_index[P[i]] = i;
       #ifdef ENABLE_LOGS
        cout << "P[" << i << "]" << " = " << P[i] << endl;
       #endif
        if(temp_is_name_of_leaf[P[i]] == false)
            b += "0";
        else
            b += "1";
    }
   #ifdef ENABLE_LOGS
    cout << "b = " << b << endl;
   #endif

    return;
}

// Returns the exit node of prefix 
string prefixToExitNodeName(string prefix)
{
    string q = prefix.substr(0, prefix.length()-prefix.length()%s);

    if( G[prefix.substr(0,F[q])] >= prefix.length())
        return prefix.substr(0,F[q]);
    else
        return prefix.substr(0, F[prefix]);
}

// takes a string present in the set P gives a value between 0 and P.size()-1 following
// a monotone minimal perfect hash function
int H(string input)
{
    return P_to_index[input];
}

int rank_b(int pos)
{
    int rankVal = 0;

    if(pos >= b.length())
        return -1;
    for(int i = 0; i < pos; i++)
        if( b[i] == '1' )
            rankVal++;

    return rankVal;
}

// returns the range of ranks of all elements that exitNodeName prefixes
pair<int, int> exitNodeNameToRanks(string exitNodeName)
{
    pair<int, int> rankRange;
    int64_t exitNodeNameVal;

    if(exitNodeName == "")
    {
        rankRange.first = 0;
        rankRange.second = n;
    }
    else
    {   
        while(exitNodeName.back() == '0')
            exitNodeName.pop_back();
        
        exitNodeNameVal = std::stoi(exitNodeName, nullptr, 2);
        rankRange.first = rank_b(H(exitNodeName));
        std::string exitNodeNamePlusOne = std::bitset< 64 >(exitNodeNameVal+1).to_string();

        int64_t temp = exitNodeNameVal;
        while (temp > 0)
        {
            // if the last bit is not set
            if ((temp & 1) == 0)
            {
                int start_index = exitNodeNamePlusOne.length()-exitNodeName.length();
                rankRange.second = rank_b(H(exitNodeNamePlusOne.substr(start_index, exitNodeName.length())));
                return rankRange;
            }
            temp = temp >> 1;
        }
        rankRange.second = n;
    }

    return rankRange;
}

pair<int, int> fastPrefixSearch(string prefix)
{
    return exitNodeNameToRanks(prefixToExitNodeName(prefix));
}
  
// Driver
int main()
{
    // Input keys (use only '0' or '1')
    string keys[] = {"001001010", "0010011010010", "00100110101"};
    string prefix = "0010011";

    n = sizeof(keys)/sizeof(keys[0]);
  
    struct TrieNode *root = getNode();

    for (int i = 0; i < n; i++)
    {
        l += keys[i].length();
        insertCompressed(root, keys[i]);
    }

    std::cout << endl;
    printCompressedTrieNodes(root);

    std::cout << endl;
    searchCompressed(root, "0010011010010", false)? std::cout << "Yes\n" :
                          std::cout << "No\n";
    searchCompressed(root, "00100110101", false)? std::cout << "Yes\n" :
                                   std::cout << "No\n";
    searchCompressed(root, "001001011", false)? std::cout << "Yes\n" :
                          std::cout << "No\n";

    searchCompressed(root, "00100110100", true)? std::cout << "Yes\n" :
                          std::cout << "No\n";

    std::cout << endl;
    s = ceil(sqrt(l / n));
    buildParam(root);

    string exitNodeTemp = prefixToExitNodeName(prefix);
    std::cout << endl;
    std::cout << "exit node for prefix \"" << prefix << "\"";
    std::cout << " is \"" << exitNodeTemp << "\"" << endl;

    pair<int,int> rankRangeTemp1 = exitNodeNameToRanks(exitNodeTemp);
    std::cout << endl;
    std::cout << "rank range for prefix \"" << prefix << "\"";
    std::cout << " is " << rankRangeTemp1.first << " to " << rankRangeTemp1.second << endl;

    pair<int,int> rankRangeTemp2 = fastPrefixSearch(prefix);
    std::cout << endl;
    std::cout << "fast prefix search for prefix \"" << prefix << "\"";
    std::cout << " is " << rankRangeTemp2.first << " to " << rankRangeTemp2.second << endl;

    return 0;
}
