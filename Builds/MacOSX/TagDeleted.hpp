//
//  TagDeleted.hpp
//  Simpler
//
//  Created by Christian White on 11/20/18.
//
//

#ifndef TagDeleted_hpp
#define TagDeleted_hpp

#include <stdio.h>
#include "TagState.h"

#endif /* TagDeleted_hpp */
class TagDeleted: public TagState{
private:
    TagState* _context;
public:
    TagDeleted(TagState* context);
    void updateTag();
};
