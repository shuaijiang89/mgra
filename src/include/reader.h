#ifndef READER_H_
#define READER_H_

#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>

#include "pconf.h"
#include "genome.h"
#include "mcolor.h"

namespace reader { 
	std::vector<Genome> read_genomes(const ProblemInstance<Mcolor>& cfg); 
	
	void read_infercars(const ProblemInstance<Mcolor>& cfg, std::vector<Genome>& genome); 

	void read_grimm(const ProblemInstance<Mcolor>& cfg, std::vector<Genome>& genome);

	std::unordered_map<std::string, std::vector<std::string> > read_cfg_file(const std::string& name_cfg_file);
	 
	__attribute__((always_inline)) inline std::string trim(std::string s, const std::string& drop = " \t\r\n"){
		s = s.erase(s.find_last_not_of(drop) + 1);
		return s.erase(0, s.find_first_not_of(drop));
	}
} 

#endif
