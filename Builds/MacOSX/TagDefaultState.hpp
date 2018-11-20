//
//  TagDefaultState.hpp
//  Simpler
//
//  Created by Christian White on 11/20/18.
//
//

#ifndef TagDefaultState_hpp
#define TagDefaultState_hpp

#include <stdio.h>
#include "TagState.h"

#endif /* TagDefaultState_hpp */

class TagDefaultState: public TagState
{
private:
    TagState* _context;
public:
    TagDefaultState(TagState* context);
    void updateTag();
};
