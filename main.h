/* CalcRuntime 
 * by Travis Wells <traviswells@mchsi.com>
 * This code is under the zlib license, see copying.txt or visit 
 * http://www.opensource.org/licenses/zlib-license.php for details.
 */

typedef unsigned long int ulint;
typedef unsigned char u8;
double CalcRunTime(ulint frames);

void ProcessOptions(int argc,char *argv[]);

ulint CalcFrames(const char *filename);

ulint b8toulint( u8 *c);

struct NECS{
	ulint pos;
	ulint firstafter;
	ulint nextafter;

};