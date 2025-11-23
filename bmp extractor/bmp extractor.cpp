/// If you would like to change algorithm for your needs notice that right side of
/// "uint16_t Sixteen_bit1 = " and "uint16_t Sixteen_bit2 = " and "uint16_t Sixteen_bit3 = " are the current pixel colors,
/// "current_mask", just before line "mask_byte = (mask_byte<<1) | current_mask;" stores mask bit
/// by Gen0me https://github.com/63n0m3/Custom_Button btc: 1A8Gqs2JwdjuAfFZ4cnM4CWy9hGFzsCpzK
#include <iostream>
//#include <bitset>
#include <fstream>
#include <string>

using namespace std;
main(){
        int32_t width = 48;
        int32_t height = 22;
        char mask_bkg;
        cout<<"This is the tool for extracting bitmaps into RGB 5/6/5 format from .bmp files."<<endl;
        cout<<"This program assumes .bmp files for conversion should be in uncompressed 24bit RGB format."<<endl;
        cout<<"Conversions were tested using MS Paint and Paint NET. All bitmap files for conversion should have names: inbut0.bmp, input1.bmp ... up to input99.bmp and be located in the same folder. Based on those files program will automatically generate bmp_arrays.h file ready to use with Custom_Button class."<<endl;
        cout<<"This software should run on big endian machine(so just a common PC/laptop)"<<endl;
        cout<<"by Gen0me"<<endl;
        cout<<"Current bugs: Width has to be a multiple of 8"<<endl;
        cout<<endl<<"Generate mask for white(w) or black(b) background:"<<endl;
        cin>>mask_bkg;
        cout<<"Mask can be switched off in Custom_Button class."<<endl;
        ofstream write ("bmp_arrays.h", ios::binary | ios::out | ios::trunc);
        int file_size;
        string array_of_image_str;
        for(int k=0; k<100; k++){
            string input_name = "input";
            string mask_str;
            input_name += to_string(k) + ".bmp";
            std::ifstream file(input_name.c_str(), ios::in | ios::binary);

            if(file.is_open()){
                file.seekg(0, ios::end);
                file_size = file.tellg();
                cout<<input_name<<"   File size: "<<file_size<<"    ";
                char read_buf[4];
                file.seekg(10, ios::beg);
                file.read(read_buf, 4);
                uint32_t Arr_starting_address_from_header = (uint32_t)read_buf[0] | ((uint32_t)read_buf[1])<< 8 | ((uint32_t)read_buf[2])<< 16 | ((uint32_t)read_buf[3])<< 24;
                file.seekg(14, ios::beg);
                file.read(read_buf, 4);
                uint32_t Determine_header_type = (uint32_t)read_buf[0] | ((uint32_t)read_buf[1])<< 8 | ((uint32_t)read_buf[2])<< 16 | ((uint32_t)read_buf[3])<< 24;
                if (Determine_header_type == 12){
                    cout<<"OS/2 1.x bitmapcoreheader"<<endl;
                    file.seekg(18, ios::beg);
                    file.read(read_buf, 4);
                    width = (uint32_t)read_buf[0] | ((uint32_t)read_buf[1])<<8;
                    height = ((uint32_t)read_buf[2])<<16 | ((uint32_t)read_buf[3])<<24;
                    cout<<"Width: "<<width<<"    Height: "<<height<<endl;
                }
                else if (Determine_header_type == 40){
                    cout<<"Windows bitmapinfoheader"<<endl;
                    file.seekg(18, ios::beg);
                    file.read(read_buf, 4);
                    width = (uint32_t)read_buf[0] | ((uint32_t)read_buf[1])<< 8 | ((uint32_t)read_buf[2])<< 16 | ((uint32_t)read_buf[3])<< 24;
                    file.seekg(22, ios::beg);
                    file.read(read_buf, 4);
                    height = (uint32_t)read_buf[0] | ((uint32_t)read_buf[1])<< 8 | ((uint32_t)read_buf[2])<< 16 | ((uint32_t)read_buf[3])<< 24;
                    cout<<"Width: "<<width<<"    Height: "<<height<<endl;
                }
                if (Arr_starting_address_from_header != file_size - width*height*3){
                    cout<<"Wrong estimate of width or high. Check if file format is RGB 8-8-8 uncompressed. Input width manually:"<<endl;
                    cin>>width;
                    cout<<"Input height manually:"<<endl;
                    cin>>height;
                    cout<<endl;
                }
                if(file_size - width*height*3 < 0){
                    cout<<"File size cannot be smaller than uncompressed array, skipping file"<<endl;
                    continue;
                }
                cout<<"Starting to read data at: "<<file_size - width*height*3<<endl;
                char pixels[width*height*3];
                file.seekg(file_size - width*height*3, ios::beg);
                file.read(pixels, width*height*3);
                file.close();
            //    cv::Mat img(64,64, CV_8UC3, pixels, 64*3);
            //    img = cv::imread("input0.bmp");
         //       cv::imshow ("ima", img);
        //        cv::waitKey(0);

                array_of_image_str = "const uint16_t bmp" + to_string(k) +"_arr_" + to_string(width) + "x" + to_string(height) + "[] PROGMEM = { ";
                mask_str.clear();
                uint8_t mask_byte = 0;
                for(int row=height-1; row>=0; row--){
                    for(int i=0; i<width; i++){
                        uint16_t Sixteen_bit1 = ((uint16_t)((uint16_t)pixels[row*width*3+3*i] & 0b11111000)>>3);//<<8
                        uint16_t Sixteen_bit2 = ((uint16_t)((uint16_t)pixels[row*width*3+3*i+1] & 0b11111100)<<3);
                        uint16_t Sixteen_bit3 = ((uint16_t)((uint16_t)pixels[row*width*3+3*i+2] & 0b11111000)<<8);//>>3
                        uint16_t Sixteen_bit = Sixteen_bit1 | Sixteen_bit2 | Sixteen_bit3;

                   //     cout<<std::bitset<16>(Sixteen_bit1)<<"   "<<std::bitset<16>(Sixteen_bit2)<<"   "<<std::bitset<16>(Sixteen_bit3)<<"   "<<std::bitset<16>(Sixteen_bit)<<"   "<<to_string (Sixteen_bit);
                //   cout<<row<<"    "<<i<<"    "<<Sixteen_bit<<"    "<<std::bitset<16>(Sixteen_bit)<<"     "<<to_string (Sixteen_bit)<<endl;
                        array_of_image_str += to_string (Sixteen_bit);
                        if (i != width-1 || row != 0) array_of_image_str += ", ";

                        uint8_t current_mask;
                        if(mask_bkg == 'w'){
                            if (Sixteen_bit >= 0xffff) current_mask = 0;
                            else current_mask = 1;
                        }
                        else if(mask_bkg == 'b'){
                            if (Sixteen_bit <= 0x0000) current_mask = 0;
                            else current_mask = 1;
                        }
                        mask_byte = (mask_byte<<1) | current_mask;
                        if( ( i % 8 == 7 || i == width - 1) && i > 0){
                            mask_str += to_string(mask_byte);
                            if (i != width-1 || row != 0) mask_str += ", ";
                            mask_byte = 0;
                        }
                    }
                }
                array_of_image_str += "};\nconst uint8_t mask" + to_string(k) + "[] PROGMEM = { " + mask_str + "};\n";
                write.write(array_of_image_str.data(), array_of_image_str.size());
            }
        }
        write.close();
        cout<<"Finished."<<endl;
}


