#include "resource.h"

////////////////////////////////////////////////////////////////////////////////
//                          Lemon 3D Graphics Engine                          //
//                    COPYRIGHT (c) 2021 by ZACH GOETHEL                      //
//  ------------------------------------------------------------------------  //
//  Use freely.  This code is published under the MIT permissive open-source  //
//  license.  For more information, see the license file included with this   //
//  repository.  Good luck, and enjoy!                                        //
//  ------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////

namespace lemon
{
    std::string read_file(std::string path, bool aggregate, std::function<void(std::string)> per_line)
    {
        std::string line, build = "";
        std::ifstream input(path);

        if (input.is_open())
        {
            while (getline(input, line))
            {
                if (aggregate)
                    build += line + "\n";
                per_line(line);
            }

            input.close();
            
            return build;
        } else
            throw std::runtime_error("Could not open requested file ('" + path + "')");
    }
}
