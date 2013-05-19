#ifndef MULARCS_H_
#define MULARCS_H_

typedef std::string vertex_t;

struct Mularcs { 
   typedef std::map<vertex_t, Mcolor> mymap; 
   typedef mymap::const_iterator citer; 
   typedef mymap::const_reverse_iterator criter;
   typedef mymap::iterator iter;    

   inline void insert(const vertex_t& v, const Mcolor& mc) { 
	mularcs.insert(std::make_pair(v, mc));	
   } 

   inline void insert(const vertex_t& v, size_t i) {
	if (mularcs.find(v) != mularcs.end()) { 
		mularcs.find(v)->second.insert(i);
	} else { 
		mularcs.insert(std::make_pair(v, Mcolor(i)));
	}
   }
 
   inline void erase(const vertex_t& v) { 
	mularcs.erase(v);
   } 
 
   inline Mcolor get_multicolor(const vertex_t& v) const {
	if (mularcs.find(v) != mularcs.cend()) { 
		return mularcs.find(v)->second; 
	} else { 
		return Mcolor();
	} 
   } 

   inline iter find(const vertex_t& v) { 
   	return mularcs.find(v);
   }

   inline citer find(const vertex_t& v) const { 
   	return mularcs.find(v);
   }
 
   inline size_t size() const {
	return mularcs.size();  
   }
 
   inline citer cbegin() const { 
	return mularcs.cbegin(); 
   } 

   inline citer cend() const { 
	return mularcs.cend();
   } 

  inline criter crbegin() const { 
	return mularcs.crbegin(); 
   } 

   inline criter crend() const { 
	return mularcs.crend();
   } 

private: 
   mymap mularcs;
}; 

#endif
