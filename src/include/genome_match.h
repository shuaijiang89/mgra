#ifndef GENOME_MATCH_ 
#define GENOME_MATCH_ 

#include <algorithm>
#include <set>
#include <sstream>
#include <cassert>

#include "pconf.h"
#include "genome.h"
#include "mcolor.h"

struct genome_match { 
  typedef std::unordered_map<orf_t, size_t> gen2num; 
 
  static void init_name_genomes(const ProblemInstance<Mcolor>& cfg, const std::vector<Genome>& genomes);
  
  inline static bool member_name (const std::string& i) { 
    return (genome_to_number.find(i) != genome_to_number.end());
  } 
	
  inline static size_t get_number(const std::string& s) {
    assert(member_name(s));
    return genome_to_number.find(s)->second;
  }

  static Mcolor name_to_mcolor(const std::string& name); 
  static std::string mcolor_to_name(const Mcolor& color);
private: 
  static std::vector<orf_t> number_to_genome;
  static gen2num genome_to_number;    		
};

  

#endif
