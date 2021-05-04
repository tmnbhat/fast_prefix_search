// Implementation of data structure defined in:
// "Fast Prefix Search in Little Space, with Applications" by Djamal Et Al.

#include <iostream>
#include <string>
#include <math.h>
#include <unordered_map>

using namespace std;

// uncomment this to enable logs
//#define ENABLE_LOGS

unordered_map <string, int> F;
unordered_map <string, int> G;
int s = 1;

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

// Recursive part of the function buildFandG
void buildFandG_recur(struct TrieNode *pCrawl, string prefixSoFar)
{
    string key = prefixSoFar;
    int prefixSoFarLength, nodeLength;

    prefixSoFarLength = prefixSoFar.length();
    nodeLength = pCrawl->node.length();

    G[prefixSoFar] = prefixSoFarLength + nodeLength;
   #ifdef ENABLE_LOGS
    std::cout << "G mapped " << prefixSoFar;
    std::cout << " to " << G[prefixSoFar] << endl;
   #endif
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
        buildFandG_recur(pCrawl->zeroChild, prefixSoFar+pCrawl->node+"0");
    }
    if(pCrawl->oneChild != NULL)
    {
        buildFandG_recur(pCrawl->oneChild, prefixSoFar+pCrawl->node+"1");
    }
}

// Build function maps F and G as described in Section 3.3 of 
// "Fast Prefix Search in Little Space, with Applications" by Djamal Et Al.
//NOTE: needs global variable "s" is be calulated beforehand 
void buildFandG(struct TrieNode *root)
{
    struct TrieNode *pCrawl = root;
    string prefixSoFar = "";
    int nodeLength = pCrawl->node.length();

    if(nodeLength != 0)
    {
        for(int i = 0; i < std::min(s, nodeLength); i++)
        {
            F[pCrawl->node.substr(0,i+1)] = 0;
           #ifdef ENABLE_LOGS
            std::cout << "mapped " << pCrawl->node.substr(0,i+1);
            std::cout << " to 0" << endl;
           #endif
        }
    }

    G[""] = nodeLength;
   #ifdef ENABLE_LOGS
    std::cout << "G mapped \"\" ";
    std::cout << " to " << nodeLength << endl;
   #endif

    prefixSoFar = pCrawl->node;

    if(root->zeroChild != NULL)
    {
        buildFandG_recur(root->zeroChild, prefixSoFar+"0");
    }
    if(root->oneChild != NULL)
    {
        buildFandG_recur(root->oneChild, prefixSoFar+"1");
    }
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
  
// Driver
int main()
{
    // Input keys (use only '0' or '1')
    string keys[] = {"001001010", "0010011010010", "00100110101"};
    string prefix = "0010011010";
    int n = sizeof(keys)/sizeof(keys[0]);
    int l = 0, s = 0;
  
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

    s = ceil(sqrt(l / n));
    buildFandG(root);

    std::cout << endl;
    std::cout << "exit node for prefix \"" << prefix << "\"";
    std::cout << " is \"" << prefixToExitNodeName(prefix) << "\"" <<endl;

    return 0;
}
