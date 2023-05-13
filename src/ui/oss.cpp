#include "ui.h"

#include <imgui.h>

struct Licence
{
    const char *library;
    const char *licence;
};

const Licence licences[]{
    {
        .library = "Raylib",
        .licence = "Copyright (c) 2013-2023 Ramon Santamaria (@raysan5)\n\
\n\
This software is provided \"as-is\", without any express or implied warranty. In no event\n\
will the authors be held liable for any damages arising from the use of this software.\n\
\n\
Permission is granted to anyone to use this software for any purpose, including commercial \n\
applications, and to alter it and redistribute it freely, subject to the following restrictions:\n\
\n\
  1. The origin of this software must not be misrepresented; you must not claim that you \n\
  wrote the original software. If you use this software in a product, an acknowledgment \n\
  in the product documentation would be appreciated but is not required.\n\
\n\
  2. Altered source versions must be plainly marked as such, and must not be misrepresented\n\
  as being the original software.\n\
\n\
  3. This notice may not be removed or altered from any source distribution.",
    },
    {
        .library = "rlImGui",
        .licence = "Copyright (c) 2020-2021 Jeffery Myers\n\
\n\
This software is provided \"as-is\", without any express or implied warranty. In no event \n\
will the authors be held liable for any damages arising from the use of this software.\n\
\n\
Permission is granted to anyone to use this software for any purpose, including commercial \n\
applications, and to alter it and redistribute it freely, subject to the following restrictions:\n\
\n\
  1. The origin of this software must not be misrepresented; you must not claim that you \n\
  wrote the original software. If you use this software in a product, an acknowledgment \n\
  in the product documentation would be appreciated but is not required.\n\
\n\
  2. Altered source versions must be plainly marked as such, and must not be misrepresented\n\
  as being the original software.\n\
\n\
  3. This notice may not be removed or altered from any source distribution.",
    },
    {
        .library = "Dear ImGui",
        .licence = "The MIT License (MIT)\n\
\n\
Copyright (c) 2014-2023 Omar Cornut\n\
\n\
Permission is hereby granted, free of charge, to any person obtaining a copy\n\
of this software and associated documentation files (the \"Software\"), to deal\n\
in the Software without restriction, including without limitation the rights\n\
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n\
copies of the Software, and to permit persons to whom the Software is\n\
furnished to do so, subject to the following conditions:\n\
\n\
The above copyright notice and this permission notice shall be included in all\n\
copies or substantial portions of the Software.\n\
\n\
THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n\
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n\
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n\
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n\
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n\
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n\
SOFTWARE.",
    },
    {
        .library = "fmt",
        .licence = "Copyright (c) 2012 - present, Victor Zverovich and {fmt} contributors\n\
\n\
Permission is hereby granted, free of charge, to any person obtaining\n\
a copy of this software and associated documentation files (the\n\
\"Software\"), to deal in the Software without restriction, including\n\
without limitation the rights to use, copy, modify, merge, publish,\n\
distribute, sublicense, and/or sell copies of the Software, and to\n\
permit persons to whom the Software is furnished to do so, subject to\n\
the following conditions:\n\
\n\
The above copyright notice and this permission notice shall be\n\
included in all copies or substantial portions of the Software.\n\
\n\
THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND,\n\
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF\n\
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND\n\
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE\n\
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION\n\
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION\n\
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n\
\n\
--- Optional exception to the license ---\n\
\n\
As an exception, if, as a result of your compiling your source code, portions\n\
of this Software are embedded into a machine-executable object form of such\n\
source code, you may redistribute such embedded portions in such object form\n\
without including the above copyright and permission notices.",
    },
    {
        .library = "nlohmann json",
        .licence = "MIT License\n\
\n\
Copyright (c) 2013-2022 Niels Lohmann\n\
\n\
Permission is hereby granted, free of charge, to any person obtaining a copy\n\
of this software and associated documentation files (the \"Software\"), to deal\n\
in the Software without restriction, including without limitation the rights\n\
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n\
copies of the Software, and to permit persons to whom the Software is\n\
furnished to do so, subject to the following conditions:\n\
\n\
The above copyright notice and this permission notice shall be included in all\n\
copies or substantial portions of the Software.\n\
\n\
THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n\
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n\
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n\
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n\
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n\
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n\
SOFTWARE.",
    },
};

namespace UI
{
    void oss()
    {
        if (bOssOpen && ImGui::Begin("Open Source Licences", &bOssOpen))
        {
            ImGui::SetWindowSize(ImVec2{600.0, 500.0}, ImGuiCond_FirstUseEver);
            for (auto licence : licences)
            {
                if (ImGui::CollapsingHeader(licence.library, ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::TextWrapped("%s", licence.licence);
                }
            }

            ImGui::End();
        }
    }
}