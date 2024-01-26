#include <iostream>
#include<fstream>
#include<iomanip>
#include<vector>
#include<algorithm>
#include<cstring>
#include<ctime>
#include <cstdint>

using namespace std;
//const int bufferSize= 20000;
const std::streamsize bufferSize = 35000 * 1024;
const size_t groupSize = 1088;//定义组长度,里面包含空格长度
const size_t followSize= 256;
size_t frameLength = 1088;
bool isValidHexChar(char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

char hexCharToByte(char c) {
    if (c >= '0' && c <= '9') {
        return static_cast<char>(c - '0');
    } else if (c >= 'A' && c <= 'F') {
        return static_cast<char>(c - 'A' + 10);
    } else if (c >= 'a' && c <= 'f') {
        return static_cast<char>(c - 'a' + 10);
    }
    return 0;
}

char hexStringToByte(const std::string& hexStr) {
    if (hexStr.size() == 2 && isValidHexChar(hexStr[0]) && isValidHexChar(hexStr[1])) {
        return (hexCharToByte(hexStr[0]) << 4) | hexCharToByte(hexStr[1]);
    }
    return 0;
}

//先计算秒数
time_t hexStringToSeconds(const std::string &hexStr){
    //首先进行判断
    if(hexStr.size()!=8)
    {
        return 0;
    }

    //按照大端排列
    uint32_t seconds;
    for (size_t i = 0; i < 4; ++i) {
        seconds = (seconds << 8) | static_cast<uint8_t>(hexStringToByte(hexStr.substr(i * 2, 2)));
    }

    return seconds;

}

bool isLeapYear1(int year){
    return (year%4==0&&year%100!=0)||(year%400==0);
}

bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}


void printData(time_t &seconds) {
    std::tm *timeinfo = std::gmtime(&seconds);  // 使用 std::gmtime 获取UTC时间

    // 获取当前年份
    int currentYear = 1970;
    while (true) {
        // 判断当前年份是否为闰年
        bool leapYear = isLeapYear(currentYear);

        // 计算当前年份的秒数
        int secondsInYear = leapYear ? 31622400 : 31536000;  // 闰年 366 天，非闰年 365 天

        // 如果剩余秒数小于当前年份的秒数，则跳出循环
        if (seconds < secondsInYear) {
            break;
        }

//        std::cout << currentYear << ": " << secondsInYear << "  leapYear:" << leapYear << std::endl;

        // 减去当前年份的秒数，增加年份
        seconds -= secondsInYear;
        ++currentYear;

    }

    // 设置时区为北京时间
    timeinfo->tm_hour += 8;
    std::mktime(timeinfo);



    cout<< std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S") << "  " ;
}



int main() {
//    std::cout << "Hello, World!" << std::endl;

    std::ifstream inputFile("C:\\Users\\HP\\Desktop\\search\\DaQi_TM__2023_12_25_CH0.dat", std::ios::binary);


//    char buffer [bufferSize];
    char* buffer = new char[bufferSize];
    if (!inputFile.is_open()) {
        std::cout << "Error opening the file" << std::endl;
        delete[] buffer;
        return 1;
    }

    if (inputFile.read(buffer, bufferSize)) {
        std::cout << "Successfully read the file into the buffer." << std::endl;
    } else {
        std::cout << "Error reading the file" << std::endl;
    }

    // Display the number of bytes read
    std::streamsize bytesRead = inputFile.gcount();
    std::cout << "You have read bytes number: " << bytesRead << std::endl;

//
//    //只读取两万字节的数据
//    if(inputFile.is_open()){
//        //通过read来读
//        inputFile.read(buffer,bufferSize);
//
//        //检查读取到的字符数是多少
//        std::streamsize bytesRead = inputFile.gcount();
//        cout<<"you have read butes numbers is "<<bytesRead<<endl;
//
//    }
//    else{
//        cout<<"Error opening the file"<<endl;
//
//    }


    //创建保存16进制的向量
    std::vector<char> hexData;
    //将数据转化为16进制

    for(std::size_t i=0;i<bufferSize;++i)
    {
        // 将输出设置为16进制，并填充为两位
        char hexChar[3];
        std::sprintf(hexChar, "%02X", static_cast<unsigned char>(buffer[i]));

        // 将十六进制字符添加到向量中
        hexData.insert(hexData.end(), hexChar, hexChar + 2);
    }


//    接下来把处理好的数据以544帧长为一组输出出去

    std::ofstream outputFile_hex("C:\\Users\\HP\\Desktop\\search\\output_hex.txt");
    if (outputFile_hex.is_open()) {
        for (size_t i = 0; i < hexData.size(); i += groupSize) {
            for (size_t j = i; j < i + groupSize && j < hexData.size(); ++j) {
                outputFile_hex.put(hexData[j]);

            }

            outputFile_hex.put('\n');
            outputFile_hex << "====\n";
        }

        std::cout << "Data successfully written to outputFile_hex.txt" << std::endl;
        outputFile_hex.close();
    } else {
        std::cerr << "outputFile_hex open ERROR!" << std::endl;
    }


    //然后读取outputFile_hex文件，开始搜索
    std::string pattern;//从这开始
    cout<<"please input pttern:"<<endl;
    cin>>pattern;
    std::ifstream inputFile_one("C:\\Users\\HP\\Desktop\\search\\output_hex.txt");
    std::ofstream outFile("C:\\Users\\HP\\Desktop\\search\\output1.txt");


    if(inputFile_one.is_open())
    {
        //先把文件以每组存进去
        std::vector<vector<char>> Groups;
        std::vector<char> Group;
        std::vector<char> Group_temp;
        char c;
        int equal_count = 0;
        size_t groupCounter = 0;

        while(inputFile_one.get(c)){
            if (c == '=') {
                equal_count++;
            } else{
                if(c != '\n'){
                    Group_temp.push_back(c);
                }
            }

            if(equal_count == 4){
                Groups.push_back(Group_temp);
                Group_temp.clear();
                equal_count = 0;
            }
        }

        for(int i=0; i<Groups.size()-1; i++){
            std::vector<char> Group_handle; // 保存第i行和i+1行
            Group_handle.insert(Group_handle.end(), Groups[i].begin(), Groups[i].end());
            Group_handle.insert(Group_handle.end(), Groups[i+1].begin(), Groups[i+1].end());

            std::string groupString(Group_handle.begin(), Group_handle.end());

            std::string hexGroup;
            for (size_t k = 0; k < groupString.size(); k += 2) {
                if (groupString.size() % 2 == 1 && k == groupString.size() - 1) {
                    std::string byte = groupString.substr(k, 1);

                    char ch = hexStringToByte(byte);
                    hexGroup.push_back(ch);
                } else {
                    std::string byte = groupString.substr(k, 2);

                    char ch = hexStringToByte(byte);
                    hexGroup.push_back(ch);
                }

            }
//            cout << groupString.size() << endl;
            //开始搜索
            size_t found = groupString.find(pattern);//数量


            if (outFile.is_open()) {
                if (found != std::string::npos) {
//                    cout << "found: " << found << endl;//1088


                    // 计算要保存的字节数
                    size_t bytesToSave = std::min(groupSize - (found + pattern.length()),
                                                  static_cast<size_t>(128));
//                    cout << groupSize - (found + pattern.length()) << endl;
//                    cout << "here" << bytesToSave;


//                    outFile << pattern;
//                    outFile << groupString.substr(found + pattern.length(), bytesToSave) << endl;
//                    cout << "here1" << bytesToSave;
//                    outFile << "----" << endl;



                    // 提取相应字节范围并转换为秒数
                    std::string hexTime;
                    size_t groupStart = groupCounter * groupSize;
//                    cout << groupStart << endl;
                    size_t byteIndex = groupStart + pattern.length() + 34 - 1;

//                    cout << byteIndex << endl;
                    size_t u = groupStart + pattern.length() + 24;
//                    cout << u;
                    for (byteIndex = groupStart + pattern.length() + 34 - 1;
                         byteIndex > groupStart + pattern.length() + 26; byteIndex -= 2) {
//                        cout << "your INdex" << byteIndex << endl;
                        hexTime.push_back(hexData[byteIndex - 1]);
                        hexTime.push_back(hexData[byteIndex]);
//                        cout << hexTime;
//                        cout << "Bytes at index " << byteIndex << ": " << hexData[byteIndex - 1]
//                             << hexData[byteIndex] << endl;
                    }

                    groupCounter++;



                    // 转换并打印日期
                    uint32_t intValue = 0;
                    std::stringstream ss(hexTime);
//                        ss << std::hex << hexTime;

                    ss >> std::hex >> intValue;

                    time_t seconds = static_cast<time_t>(intValue);
//                        cout <<"seconds"<<seconds << endl;
                    printData(seconds);
                    cout << pattern;
                    cout << groupString.substr(found + pattern.length(), bytesToSave);




                    // 如果需要继续保存下一组的数据
                    size_t remainingBytes = 128 - bytesToSave;
                    size_t nextGroupIndex = groupSize;
                    if(remainingBytes <= 0 || nextGroupIndex >= groupString.size())
                    {
                        cout<<endl;
                    }

                    while (remainingBytes > 0 && nextGroupIndex < groupString.size()) {
//                        // 提醒已跨越到下一组
//                        cout << "Crossed to the next group!" << endl;

                        // 打印下一组的 pattern 和后面的字节
                        size_t nextGroupBytes = std::min(groupSize, remainingBytes);
                        cout << groupString.substr(nextGroupIndex, nextGroupBytes) << std::endl;

                        // 更新剩余字节数和下一组的索引
                        remainingBytes -= nextGroupBytes;
                        // 提醒已跨越到下一组
                        cout << "Crossed to the next group!" << endl;
                    }
                } else {
                    cout << "error found: " << found << endl;
                    std::cout << "Target string not found in the group.\n";
                }
            } else {
                cout << "the output1.txt is error to open";
            }
        }
        

        inputFile_one.close();
    }else{
        std::cerr<<"Unable to open the file"<<endl;
    }

    return 0;
}
