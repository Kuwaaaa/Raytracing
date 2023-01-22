#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main(){
    string fname = "testname";
    string lname = "123";
    double height = 1.6;

    string fname2 = "short";
    string lname2 = "123";
    double height2 = 1.8;


    std::vector<std::string> const first_column_entries
{
    "Namaaaaaaaaaaaaaaaaaaaaaaaaaaaae",
    fname + " " + lname,
    fname2 + " " + lname2
};

auto const width_of_longest_entry = std::max_element(std::begin(first_column_entries), std::end(first_column_entries),
    [](std::string const& lhs, std::string const& rhs)
    {
        return lhs.size() < rhs.size();
    }
)->size();

// some margin:
auto const column_width = width_of_longest_entry + 3;

std::cout << std::left << std::setw(column_width) << "Name" << "Height[m]" << "\n";
std::cout << std::left << std::setw(column_width) << fname + " " + lname << std::setprecision(2) << std::fixed << height << "\n";
std::cout << std::left << std::setw(column_width) << fname2 + " " + lname2 << std::setprecision(2) << std::fixed << height2 << "\n";
}