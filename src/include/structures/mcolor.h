#ifndef MCOLOR_H_
#define MCOLOR_H_

#include <iostream>
#include <algorithm> 
#include <map>
#include <set>
#include <utility>

#include "equivalence.h"

#define member(S,x) ((S).find(x)!=(S).end()) //FIXME: DELETED

struct Mcolor {
  enum Construct {Difference, Union, Intersection};
	
  typedef std::map<size_t, size_t> my_map;
  typedef my_map::const_iterator citer;
  typedef my_map::iterator iter; 

  Mcolor() { 
  } 

  Mcolor(size_t i) {
    main_color.insert(std::make_pair(i, 1));
  }  

  Mcolor(const Mcolor& first, const Mcolor& second, const Construct& what) { 
    switch (what) { 
    case Difference: set_difference(first, second);	
      break;
    case Union: set_union(first, second); 
      break; 
    case Intersection: set_intersection(first, second);
      break; 
    } 
  } 

  inline bool is_one_to_one_match() const {
    for (auto it = main_color.cbegin(); it != main_color.cend(); ++it) { 
      if (it->second != 1) { 
	return false; 
      } 
    } 
    return true; 
  }
	
  inline size_t how_much_includes(const Mcolor& second) const {
    size_t answer = 0;
    Mcolor current = *this; 
    while (current.includes(second)) {
      ++answer; 
      current = Mcolor(current, second, Mcolor::Difference);
    }  	
    return answer; 
  }
	
  inline bool includes(const Mcolor& second) const { 	
    auto first1 = main_color.cbegin(); 
    auto first2 = second.cbegin(); 

    for (; first2 != second.cend(); ++first1) { 
      if (first1 == main_color.cend()) {
	return false;
      }

      if (first2->first < first1->first) { 
	return false;
      } 
	
      if (first1->first == first2->first) { 
	if  (first1->second < first2->second) {
	  return false; 
	} else {
	  ++first2; 
	}
      } 
    } 
    return true;
  } 


  inline void insert(size_t i) {
    if (main_color.find(i) == main_color.end()) {  
      main_color.insert(std::make_pair(i, 1));
    } else {
      main_color[i] += 1;	
    } 
  } 

  inline size_t find(size_t i) { 
    return main_color[i];					
  } 

  inline bool in_color(size_t j) const { 
    return (main_color.find(j) != main_color.end());
  } 

  inline bool empty() const { 
    return main_color.empty();
  } 

  inline size_t size() const { 
    return main_color.size();
  } 
	
  inline iter begin() { 
    return main_color.begin();
  } 

  inline iter end() { 
    return main_color.end();
  } 

  inline citer cbegin() const { 
    return main_color.cbegin();
  } 

  inline citer cend() const { 
    return main_color.cend();
  } 

  bool operator > (const Mcolor& C) const { 
    return (main_color > C.main_color);
  } 

  bool operator < (const Mcolor& C) const {
    return (main_color < C.main_color); 
  } 

  bool operator == (const Mcolor& C) const { 
    return (main_color == C.main_color);	
  } 

  bool operator != (const Mcolor& C) const { 
    return (main_color != C.main_color); 
  }
	
private:
  void set_difference(const Mcolor& first, const Mcolor& second) {
    auto first1 = first.cbegin(); 
    auto first2 = second.cbegin(); 
    auto result = std::inserter(main_color, main_color.begin());

    while (first1 != first.cend() && first2 != second.cend()) {
      if (first1->first == first2->first) { 
	if (first1->second > first2->second) { 
	  *result = std::make_pair(first1->first, first1->second - first2->second);
	} 
	++first1;
	++first2;
      } else if (*first1 < *first2) {
	*result = *first1; 
	++result; 
	++first1; 
      } else if (*first2 < *first1) { 
	++first2;
      } 
    }
	
    for (; first1 != first.cend(); ++first1, ++result) { 
      *result = *first1;
    } 
  } 
	
  void set_union(const Mcolor& first, const Mcolor& second) {
    auto first1 = first.cbegin(); 
    auto first2 = second.cbegin(); 
    auto result = std::inserter(main_color, main_color.begin());
	
    while (true) { 
      if (first1 == first.cend()) { 
	for (; first2 != second.cend(); ++first2, ++result) { 
	  *result = *first2;
	} 
	break; 
      } 
		
      if (first2 == second.cend()) { 	
	for (; first1 != first.cend(); ++first1, ++result) { 
	  *result = *first1;
	} 		
	break;
      } 
	
      if (first1->first == first2->first) { 
	*result = std::make_pair(first1->first, first1->second + first2->second);	
	++first1; 
	++first2; 
      } else if (*first1 < *first2) {
	*result = *first1; 
	++first1; 
      } else if (*first2 < *first1) { 
	*result = *first2; 
	++first2;			
      }  
      ++result;
    } 	
  }
	
  void set_intersection(const Mcolor& first, const Mcolor& second) {
    auto first1 = first.cbegin(); 
    auto first2 = second.cbegin(); 
    auto result = std::inserter(main_color, main_color.begin());

    while (first1 != first.cend() && first2 != second.cend()) {
      if (first1->first == first2->first) { 
	*result = std::make_pair(first1->first, std::min(first1->second, first2->second)); 		
	++result; 
	++first1;
	++first2;
      } else if (*first1 < *first2) { 
	++first1;
      } else if (*first2 < *first1) { 
	++first2;
      } 
    }
  }

private: 
  my_map main_color;
};
#endif
