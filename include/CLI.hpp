#pragma once

/*
 *   shapr --input model.obj --output model.stl
 *        --translate x,y,z
 *        --rotate angle,x,y,z
 *        --scale x,y,z (or single value)
 *        --inside x,y,z
 *        --surface-area
 *        --volume
 */

class CLI
{
public:
    static int Run(int argc, const char* argv[]);
};
