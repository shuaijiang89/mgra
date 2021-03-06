#ifndef ESTIMATE_H_
#define ESTIMATE_H_

#include <set>
#include <map>
#include <string>
#include <vector>
#include <cstdlib>

#include "mcolor.h"

template<class graph_t>
struct Statistics { 
  Statistics(const graph_t& gr)
  : graph(gr) 
  {
   
    count_compl_multiedges();
  
    //count_chromosomes();  
    count_all();
  }	      	
 
  inline void count_other() {
    count_cycles();
  }

  std::vector<size_t> count_all() const;
  std::vector<std::string> get_compl_stat() const;   
  std::vector<std::string> get_no_compl_stat() const;	
  std::vector<Mcolor> get_new_color() const;

  std::map<std::pair<Mcolor, Mcolor>, size_t> get_Hsubgraph(); //count H-subgraph for stage2 

private:
  void count_compl_multiedges(); //count good edges for stage1
  void count_not_compl_multiedges(); 
  void count_cycles();
  void count_chromosomes();

  __attribute__((always_inline)) inline size_t calc_value(const std::map<Mcolor, size_t>& where, const Mcolor& what) const { 
    if (where.find(what) != where.end()) { 
      return where.find(what)->second;
    } 
    return 0; 
  } 

private: 
  const graph_t& graph;
  
  //vertices
  std::unordered_map<size_t, size_t > multidegree_count; // multidegree_count[n] = # vertices of multidegree n. 
  std::map<Mcolor, size_t> simple_vertices_count;  	 // simple_vertices_count[min(S,!S)] = # simple vertices incident to S-colored
  std::map<Mcolor, size_t> simple_vertices_alone_count;  // simple_vertices_alone_count[min(S,!S)] = # simple vertices incident to S-colored, with no good neighbors

  //edges
  std::map<Mcolor, size_t> not_compl_multiedges_count;	// not complement multiedges[S] = # multiedges of not complement multicolor S,
  std::map<Mcolor, size_t> compl_multiedges_count; 	// multiedges_count[S] = # multiedges of multicolor S.
  std::map<Mcolor, size_t> good_multiedges_count; 	// good_multiedges_count[S] = # good multiedges of multicolor S. 
  std::map<Mcolor, size_t> good_irrer_multiedges_count;	// ME[S] = # good irregular multiedges of multicolor S.
  std::map<Mcolor, size_t> simple_multiedges_count;	// ME[S] = # simple multiedges of multicolor S.

  std::map<std::pair<Mcolor, Mcolor>, size_t> Hcount; // count H-subgraphs
  //std::map<std::pair<Mcolor, Mcolor>, bool> Hmid;   // middle edge is T-consistent?
	
  //cycles
  std::map<Mcolor, size_t> simple_cycle_count; 		// cycle of simple vertices
  std::map<Mcolor, size_t> special_cycle_count; 	// cycle of simple vertices and oo, of even length

  //chromosome
  std::vector<size_t> liniar_chr; 				
  std::vector<size_t> circular_chr; 				
};

template<class graph_t>
std::vector<size_t> Statistics<graph_t>::count_all() const {
  size_t dupl = 0; 
  size_t indel = 0; 
  size_t count_self_loop = 0;
  size_t dupl_mcolors = 0;  	

  for(auto it = graph.begin_vertices(); it != graph.end_vertices(); ++it) {
    if (graph.is_duplication_vertex(*it)) {
	++dupl;
    } else if (graph.is_indel_vertex(*it)) {
	++indel;
    } 

    if (graph.is_have_self_loop(*it)) {
	++count_self_loop;
    } 

    Mularcs<Mcolor> current = graph.get_adjacent_multiedges(*it); //current is list with adjacent multiedges
    for (auto it = current.cbegin(); it != current.cend(); ++it) {
	if (!it->second.is_one_to_one_match()) {
		++dupl_mcolors;
	}
    } 
  } 

  std::vector<size_t> answer({dupl, indel, count_self_loop, dupl_mcolors});
  return answer;
} 

template<class graph_t>
std::vector<std::string> Statistics<graph_t>::get_compl_stat() const { 
  std::multimap<size_t, std::string> answer;

  for(auto im = compl_multiedges_count.cbegin(); im != compl_multiedges_count.cend(); ++im) {
    const Mcolor& current = graph.get_complement_color(im->first);  // complementary multicolor.

    if (im->first < current) {
      continue;
    } 
	 
    size_t m1 = calc_value(compl_multiedges_count, current) / 2;
    size_t m2 = (im->second) / 2;	
    int paths = calc_value(simple_vertices_count, current) - (calc_value(simple_multiedges_count, current) + calc_value(simple_multiedges_count, im->first)) - calc_value(simple_vertices_alone_count, current) - calc_value(special_cycle_count, current);
    int cycles = calc_value(simple_cycle_count, current) + calc_value(special_cycle_count, current);
 	
    std::ostringstream os;
 
    os << "{";		    
    if (graph.is_T_consistent_color(im->first)) { 
      os << "\\bf ";
    } 

    const Mcolor& first = graph.get_min_complement_color(current); 
    const Mcolor& second = graph.get_complement_color(first);

    os <<  genome_match::mcolor_to_name(first) << " + "  <<  genome_match::mcolor_to_name(second) << "} & " 
      // multiedges
       << m1 << " + " << m2 << " = " << m1 + m2 << " & " 
      // simple vertices
       << calc_value(simple_vertices_count, current) << " & "  
      // simple multiedges
       << calc_value(simple_multiedges_count, current) << " + " << calc_value(simple_multiedges_count, im->first) << " = " << calc_value(simple_multiedges_count, current) + calc_value(simple_multiedges_count, im->first) << " & "
      // simple paths + cycles
       << paths << " + " << cycles << " = " << paths + cycles << " & "
      // irregular multiedges
       << calc_value(good_irrer_multiedges_count, current) << " + " << calc_value(good_irrer_multiedges_count, im->first) << " = " << calc_value(good_irrer_multiedges_count, current) + calc_value(good_irrer_multiedges_count, im->first);
	
	//std::cerr <<  calc_value(simple_vertices_count, current) << " " << calc_value(simple_multiedges_count, current) << " " << calc_value(simple_multiedges_count, im->first) << " " << calc_value(simple_vertices_alone_count, current) << " " << calc_value(special_cycle_count, current) << std::endl;
    answer.insert(std::make_pair(m1 + m2, os.str()));
  }
	
  std::vector<std::string> output;
  for(auto it = answer.rbegin(); it != answer.rend(); ++it) {
    output.push_back(it->second); 
  }  	
  return output;
} 

template<class graph_t>
std::vector<std::string> Statistics<graph_t>::get_no_compl_stat() const { 
  std::multimap<size_t, std::string> answer;

  for(auto im = not_compl_multiedges_count.cbegin(); im != not_compl_multiedges_count.cend(); ++im) {
    size_t vm1 = im->second;
		
    std::ostringstream os;
  
    os << "{";		    
    if (graph.is_T_consistent_color(im->first)) { 
      os << "\\bf ";
    } 
    
    os <<  genome_match::mcolor_to_name(im->first) << "} & " << vm1;

    answer.insert(std::make_pair(vm1, os.str()));
  } 

  std::vector<std::string> output;
  for(auto it = answer.rbegin(); it != answer.rend(); ++it) {
    output.push_back(it->second); 
  }  	
  return output;	
}

template<class graph_t>
std::vector<Mcolor> Statistics<graph_t>::get_new_color() const { 
  std::vector<Mcolor> output(compl_multiedges_count.size());
  for(auto im = compl_multiedges_count.cbegin(); im != compl_multiedges_count.cend(); ++im) { 
    output.push_back(im->first);
  }  
  return output;	
} 

template<class graph_t>
void Statistics<graph_t>::count_compl_multiedges() {
  std::unordered_set<std::string> processed;

  for(auto it = graph.begin_vertices(); it != graph.end_vertices(); ++it) {
    Mularcs<Mcolor> current = graph.get_adjacent_multiedges(*it); //current is list with adjacent multiedges

    ++multidegree_count[current.size()]; //current.size - is degree vertex *it

    if (graph.is_simple_vertex(*it)) {  //we define simple vertices as a regular vertex of multidegree 2. 
      processed.insert(*it);
      ++simple_vertices_count[std::min(current.cbegin()->second, current.crbegin()->second)]; //simple vertices because degree 2.
    }

    for(auto im = current.cbegin(); im != current.cend(); ++im) {
      if (!im->second.is_one_to_one_match()) {  /*|| !graph.is_T_consistent_color(im->second)) { */
	continue;
      } 

      ++compl_multiedges_count[im->second];   // count two times, because same underected edge (u, v) and (v, u)
			
      if (graph.is_simple_vertex(*it)) {
	++good_multiedges_count[im->second]; //good if one vertices have degree 2
	
	if (im->first == Infty) { 
	  ++good_multiedges_count[im->second];
	} 
						
	if (processed.find(im->first) != processed.end()) {  
	  ++simple_multiedges_count[im->second]; //if two vertices have degree = 2 - is simple edges
	} 
      } 

      if (im->first == Infty) { 
	++compl_multiedges_count[im->second];
	++good_irrer_multiedges_count[im->second];			
      } 
    }
  }

  // count lonely vertices (short paths) 
  for(auto it = processed.cbegin(); it != processed.cend(); ++it) {
    Mularcs<Mcolor> current = graph.get_adjacent_multiedges(*it);
    if (processed.find(current.cbegin()->first) == processed.end() && processed.find(current.crbegin()->first) == processed.end()) {
      ++simple_vertices_alone_count[std::min(current.cbegin()->second, current.crbegin()->second)]; //no good neighbors
    }
  } 
} 

template<class graph_t>
void Statistics<graph_t>::count_not_compl_multiedges() { 
  for(auto it = graph.begin_vertices(); it != graph.end_vertices(); ++it) {
    Mularcs<Mcolor> current = graph.get_adjacent_multiedges(*it);  
    for (auto jt = current.cbegin(); jt != current.cend(); ++jt) {
      if (jt->second.is_one_to_one_match()) {
	continue; 
      } 

      ++not_compl_multiedges_count[jt->second]; 

      if (jt->first == Infty) { 
	++not_compl_multiedges_count[jt->second];		
      } 
    } 				 
  } 	
} 

template<class graph_t>
std::map<std::pair<Mcolor, Mcolor>, size_t> Statistics<graph_t>::get_Hsubgraph() { 
  std::unordered_set<vertex_t> processed;
	
  for(auto is = graph.begin_vertices(); is != graph.end_vertices(); ++is) {
    Mularcs<Mcolor> Mx = graph.get_adjacent_multiedges(*is);
  
    bool is_fair1 = (Mx.size() == 3); 
    if (is_fair1) {
      for(auto it = Mx.cbegin(); it != Mx.cend(); ++it) {
	if (!it->second.is_one_to_one_match()) { 
	  is_fair1 = false;
	}
      }  
    }
  
    if (is_fair1) { 
      for(auto im = Mx.cbegin(); im != Mx.cend(); ++im) {
	if (im->first == Infty || processed.find(im->first) != processed.end()) { 
	  continue; 
	} 

	Mularcs<Mcolor> My = graph.get_adjacent_multiedges(im->first);

        bool is_fair2 = (My.size() == 3); 
        if (is_fair2) {
          for(auto it = Mx.cbegin(); it != Mx.cend(); ++it) {
	    if (!it->second.is_one_to_one_match()) { 
	      is_fair2 = false;
	    }
          }   
        }
			
	if (is_fair2) {
	  Mularcs<Mcolor> Mx0 = Mx;
	  
	  Mx0.erase(im->first);
	  My.erase(*is);
	  
	  //Mcolor Q1 = Mx0.begin()->second;
	  //Mcolor Q2 = Mx0.rbegin()->second;
	  if ((Mx0.cbegin()->second == My.cbegin()->second) || (Mx0.crbegin()->second == My.cbegin()->second)) {
	    Mcolor QQ1 = graph.get_min_complement_color(Mx0.cbegin()->second);
	    Mcolor QQ2 = graph.get_min_complement_color(Mx0.crbegin()->second);	    
	    ++Hcount[std::make_pair(QQ1, QQ2)];
	    ++Hcount[std::make_pair(QQ2, QQ1)];
	    //Hmid[std::make_pair(QQ2, QQ1)] = MBG.is_T_consistent_color(im->second);
	  }
	} 
      } 
    }
    processed.insert(*is);
  }

   return Hcount;	
} 

template<class graph_t>
void Statistics<graph_t>::count_cycles() { 
  std::unordered_set<std::string> processed;

  for(auto is = graph.begin_vertices(); is != graph.end_vertices(); ++is) {
    if (processed.find(*is) != processed.end()) { 
      continue; 
    } 

    Mularcs<Mcolor> Mx = graph.get_adjacent_multiedges(*is); 

    if (!(graph.is_simple_vertex(*is) && graph.get_complement_color(Mx.cbegin()->second) == Mx.crbegin()->second)) { 
      continue;
    } 

    std::string current = *is;
    std::string prev = "";
    Mcolor special_Q; 

    do {
      processed.insert(current);

      if (!graph.is_simple_vertex(current)) {
	break;
      }

      Mularcs<Mcolor> My = graph.get_adjacent_multiedges(current);

      if (prev == My.cbegin()->first) {
	prev = current;
	current = My.crbegin()->first;
      } else {
	prev = current;
	current = My.cbegin()->first;
      }

      while (current == Infty) {
	if (special_Q.empty()) {
	  special_Q = My.find(current)->second;
	  prev = *is;
	  current = Mx.cbegin()->first; 
	} else {
	  if (special_Q != My.find(current)->second) { 
	    ++special_cycle_count[std::min(special_Q, My.find(current)->second)]; 	  
	  }
	  break;
	}
      }
    } while ((current != Infty) && (processed.find(current) == processed.end()));
	
    if (current == *is) { //find cycle. 
      ++simple_cycle_count[std::min(Mx.cbegin()->second, Mx.crbegin()->second)];
    }
  }
} 

template<class graph_t>
void Statistics<graph_t>::count_chromosomes() { 
  circular_chr.resize(graph.size_graph());
  liniar_chr.resize(graph.size_graph());
  
  for(int i = 0; i < graph.size_graph(); ++i) {
    std::unordered_set<orf_t> processed;		

    for(auto is = graph.begin_vertices(); is != graph.end_vertices(); ++is) {		    
      if (processed.find(*is) == processed.end()) { 				  
	processed.insert(*is);
	std::string y = graph.get_obverse_vertex(*is);

	while (true) {
	  if (member(processed, y)) {
	    ++circular_chr[i];
	    break;
	  }
	  
	  processed.insert(y);
	  if (!graph.is_exist_edge(i, y)) {
	    ++liniar_chr[i];
	    break;
	  }
	  
	  y = graph.get_adjecent_vertex(i, y);
	  if (member(processed, y)) {
	    ++circular_chr[i];
	    break;
	  }
	  processed.insert(y);
	  y = graph.get_obverse_vertex(y);
	}
		
	if (graph.is_exist_edge(i, *is)) {
	  std::string y = graph.get_adjecent_vertex(i, *is);
					
	  while (processed.find(y) == processed.end()) {
	    processed.insert(y);
	    y = graph.get_obverse_vertex(y);
	    if (member(processed, y)) { 
	      break;
	    } 

	    processed.insert(y);
	    if (!graph.is_exist_edge(i, y))  { 
	      break;
	    } 
	    y = graph.get_adjecent_vertex(i, y);
	  }
	}		    
      }	
    } 
  }
} 

#endif
