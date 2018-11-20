//
//  TagSelected.hpp
//  Simpler
//
//  Created by Christian White on 11/20/18.
//
//

#ifndef TagSelected_hpp
#define TagSelected_hpp

#include <stdio.h>
#include "TagState.h"

#endif /* TagSelected_hpp */

class TagSelected: public TagState
{
private:
    TagState* _context;
public:
    TagSelected(TagState* context);
    void updateTag();
};
