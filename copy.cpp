#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

int main(int argc, char* argv[])
{
	   //Error
       if(argc < 3)
       {
       		std::cout << "Not enough arguments";
       		return -1;
       }
       
       int source = open(argv[1], O_RDONLY);//source file
       int destination = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);//destination file

	   //Size of the files
       int source_log_size = 0, source_phys_size = 0, destination_log_size = 0, destination_phys_size = 0; 
	   long offset = 0, hole_pointer = 0, data_pointer = 0;
	   
	   char* buffer = new char[0];
	   long buffer_size = 0; 
	   
       while(1)
       {
       		hole_pointer = lseek(source, offset, SEEK_HOLE);
       		source_log_size += hole_pointer - data_pointer;
       		
 			offset = hole_pointer;  
 			
 			buffer_size = hole_pointer - data_pointer;
 			buffer = new char[buffer_size];

 			lseek(source, -buffer_size, SEEK_CUR);
 		    read(source, buffer, buffer_size);
 			
 			
 			destination_log_size += write(destination, buffer, buffer_size);
 			
 			data_pointer = lseek(source, offset, SEEK_DATA);
 		    
 		    
 			if(data_pointer == -1)
 			{
 				if(errno == ENXIO)
 				{
 					data_pointer = lseek(source, 0, SEEK_END);
 				    lseek(destination, data_pointer - hole_pointer, SEEK_END);
 				    destination_phys_size += data_pointer - hole_pointer;				
 					break;
 				}
 				else
 				{
 					return -1;
 				}
 			}
 			source_phys_size += data_pointer - hole_pointer;
 			
 		    //copy the "holes" of files		
 			offset = data_pointer;
 			lseek(destination, data_pointer - hole_pointer, SEEK_END);
 			destination_phys_size += data_pointer - hole_pointer;
       }
       
       std::cout << "Source file logical size: " << source_log_size << ", source file physical size: " << source_log_size + source_phys_size << "\n";
       std::cout << "Destination file logical size: " << destination_log_size << ", destination file physical size: " << destination_log_size + destination_phys_size << "\n";
}
