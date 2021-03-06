#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

struct Ratio
{
    int num;
    int den;
    
    Ratio()
        : num(0)
        , den(0)
    {}
    
    void reduce()
    {
        for (int i=min(num, den); i>1; --i)
        {
            if (num%i == 0 && den%i == 0)
            {
                num /= i;
                den /= i;
                return;
            }
        }
    }
};

struct Item;

struct ItemConv
{
    Item* to;
    Ratio rate;
    
    ItemConv()
        : to(NULL)
        , rate()
    {}
};

struct Item
{
    map<string, ItemConv> conv;
    string name;
    bool visited;
    
    Item()
        : conv()
        , name()
        , visited(false)
    {}
    
    ItemConv convert(string const& to)
    {
        typedef map<string, ItemConv>::iterator MIter;
        
        ItemConv rv;
        ItemConv tmp;
        
        if (visited) return rv;
        
        // Search direct conversions
        
        for (MIter i=conv.begin(), ie=conv.end(); i!=ie; ++i)
        {
            if (i->first == to)
            {
                rv = i->second;
                return rv;
            }
        }
        
        // No direct conversion, try indirect conversions5.
        
        visited = true;
        
        for (MIter i=conv.begin(), ie=conv.end(); i!=ie; ++i)
        {
            tmp = i->second.to->convert(to);
            
            if (tmp.to)
            {
                rv = i->second;
                rv.to = tmp.to;
                rv.rate.num *= tmp.rate.num;
                rv.rate.den *= tmp.rate.den;
                
                // Link for efficiency
                
                conv[to] = rv;
                
                Item* target = tmp.to;
                tmp.to = this;
                swap(tmp.rate.num, tmp.rate.den);
                target->conv[name] = tmp;
                
                break;
            }
        }
        
        visited = false;
        return rv;
    }
};

int main()
{
    ifstream in ("exchange.in");
    
    map<string, Item> items;
    
    string str;
    
    int a;
    int b;
    string astr;
    string bstr;
    
    while (in >>str && str!=".")
    {
        if (str == "!")
        {
            in >>a >>astr >>str >>b >>bstr;
            
            Item& aitem = items[astr];
            Item& bitem = items[bstr];
            
            if (aitem.name.empty()) aitem.name = astr;
            if (bitem.name.empty()) bitem.name = bstr;
            
            ItemConv& aconv = aitem.conv[bstr];
            ItemConv& bconv = bitem.conv[astr];
            
            aconv.to = &bitem;
            bconv.to = &aitem;
            
            aconv.rate.num = b;
            aconv.rate.den = a;
            
            bconv.rate.num = a;
            bconv.rate.den = b;
        }
        else
        {
            in >>astr >>str >>bstr;
            
            Item& aitem = items[astr];
            
            ItemConv conv = aitem.convert(bstr);
            
            conv.rate.reduce();
            
            if (conv.to)
            {
                cout <<conv.rate.den <<" " <<astr <<" = " <<conv.rate.num <<" " <<bstr <<endl;
            }
            else
            {
                cout <<"? " <<astr <<" = ? " <<bstr <<endl;
            }
        }
    }
}
