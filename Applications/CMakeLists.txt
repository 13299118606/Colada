#============================================================================
#
# Copyright (c) Kitware, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0.txt
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#============================================================================

# add_subdirectory(ColadaCore)
# add_subdirectory(ColadaQT)
add_subdirectory(ColadaApp)

add_subdirectory(Python)

# Since "Python" depends on "ColadaApp", the folder is included in the top-level CMakeLists.txt
# after "ColadaApp" folder.