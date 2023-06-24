//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

simple Node
{
    parameters:
        double x = default(0);// = default(uniform(0m, 15000m));                 // the x coordinate of the host
        double y = default(0);// = default(uniform(0m, 15000m));                 // the y coordinate of the host
        int id;
        int loraType;
        int deviceNum;
        int maxTransDist;
        bool ifDead @mutable;
        int fatherID @mutable;
        double battery @mutable;
        @display("t=Not networked");
        @display("i=device/accesspoint_s;p=$x,$y");
    gates:
        input port @directIn;
}
