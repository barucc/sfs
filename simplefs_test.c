#include "bitmap.c"
#include "disk_driver.c"
#include "simplefs.c"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h>
#include <stdlib.h>

#define BLOCKS 1000
#define TEST_PATH "mydisk.txt"


int main(int argc, char** argv) {
	
	if(argc < 2){
		printf("\033[0;31m"); 
		printf("\nUsage: ./simplefs_test.c <code>\n");
		printf("\033[0m"); 
		printf("\nIf you want to test the bitmap module's functions: code = bitmap\n");
		printf("\nIf you want to test the disk driver module's functions: code = disk_driver\n");
		printf("\nIf you want to test the file system: code = simplefs\n");
		return 0;
	}
	
	char* test = argv[1];
	
	//BITMAP TEST
	if(strcmp(test, "bitmap") == 0){
		printf("BITMAP FUNCTIONS TEST\n");
		
		// BitMap_blockToIndex(int num)
		int pos = BLOCKS;
		printf("\n*** Testing BitMap_blockToIndex(%d) ***\n", pos);   
		BitMapEntryKey block_info = BitMap_blockToIndex(pos);
		printf("\nBlock position = %d has been converted to entry number = %d, bit offset = %d\n", pos, block_info.entry_num, block_info.bit_num);
		printf("{Expected: 124, 7}\n");
	 
		// BitMap_indexToBlock(int entry, uint8_t bit_num)
		printf("\n*** Testing BitMap_indexToBlock(%d, %d) ***\n", block_info.entry_num, block_info.bit_num);
		pos = BitMap_indexToBlock(block_info.entry_num, block_info.bit_num); 
		printf("\nBitmap entry key (%d, %d) has been converted to block position = %d\n", block_info.entry_num, block_info.bit_num, pos);
		printf("{Expected: 999}\n");

		// BitMap_get(BitMap* bmap, int start, int status)
		// BitMap_set(BitMap* bmap, int pos, int status)
		printf("\n*** Testing BitMap_get(BitMap* bmap, int start, int status) ***\n");
		printf("*** Testing BitMap_set(BitMap* bmap, int pos, int status) ***\n");
		
		BitMap* bitmap = (BitMap*) malloc(sizeof(BitMap));
		bitmap->entries = (char*) calloc(BLOCKS/8,sizeof(char));
		bitmap->num_bits = BLOCKS;
		printf("\nNumber of blocks = %d, number of bitmap entries = %d\n", BLOCKS, BLOCKS/8); 
		
		printf("\nBitMap_get returns the index of the first block in a status, -1 otherwise\n");
		printf("\nBitMap_set returns the new status of the block, -1 otherwise\n");
		
		printf("\nBitMap_get(bitmap, 0, 0) returns -> %d    {Expected: 0}\n", BitMap_get(bitmap, 0, 0));
		printf("BitMap_get(bitmap, 50, 0) returns -> %d    {Expected: 50}\n", BitMap_get(bitmap, 50, 0));
		printf("BitMap_get(bitmap, 998, 0) returns -> %d    {Expected: 998}\n", BitMap_get(bitmap, 998, 0));
		printf("BitMap_get(bitmap, 8000, 0) returns -> %d    {Expected: -1}\n", BitMap_get(bitmap, 8000, 0));

		printf("BitMap_get(bitmap, 0, 1) returns -> %d    {Expected: -1}\n", BitMap_get(bitmap, 0, 1));
		
		printf("BitMap_set(bitmap, 0, 1) returns -> %d    {Expected: 1}\n", BitMap_set(bitmap, 0, 1));
		printf("BitMap_set(bitmap, 998, 1) returns -> %d    {Expected: 1}\n", BitMap_set(bitmap, 998, 1));
		printf("BitMap_set(bitmap, 8000, 0) returns -> %d    {Expected: -1}\n", BitMap_set(bitmap, 8000, 0));
		
		printf("BitMap_get(bitmap, 50, 1) returns -> %d    {Expected: 998}\n", BitMap_get(bitmap, 50, 1));
		
		printf("BitMap_set(bitmap, 50, 1) returns -> %d    {Expected: 1}\n", BitMap_set(bitmap, 50, 1));
		
		printf("BitMap_get(bitmap, 0, 1) returns -> %d    {Expected: 0}\n", BitMap_get(bitmap, 0, 1));
		
		printf("BitMap_set(bitmap, 0, 0) returns -> %d    {Expected: 0}\n", BitMap_set(bitmap, 0, 0));
		
		printf("BitMap_get(bitmap, 0, 0) returns -> %d    {Expected: 0}\n", BitMap_get(bitmap, 0, 0));
	
		printf("\nDestroying bitmap\n");
		BitMap_destroy(bitmap);
		
	}
	//DISK DRIVER TEST
	else if(strcmp(test, "disk_driver") == 0){
		printf("DISK DRIVER FUNCTIONS TEST\n");
		
		// DiskDriver_init(DiskDriver* disk, const char* filename, int num_blocks)
		printf("\n*** Testing DiskDriver_init(DiskDriver* disk, const char* filename, int num_blocks) ***\n");
		DiskDriver* disk = (DiskDriver*) malloc(sizeof(DiskDriver));
		DiskDriver_init(disk, TEST_PATH, BLOCKS);
		
		int block_num = disk->header->first_free_block;
		
		if(block_num == -1){
			printf("Disk driver initialization error\n");
			return 0;
		}
		
		printf("\nDisk driver and bitmap correctly initialized\n");
		printf("First free block index = %d\n", block_num);
		
		// DiskDriver_readBlock(DiskDriver* disk, void* dest, int block_num)
		// DiskDriver_writeBlock(DiskDriver* disk, void* src, int block_num)
		printf("\n*** Testing DiskDriver_readBlock(DiskDriver* disk, void* dest, int block_num) ***\n");
		printf("\n*** Testing DiskDriver_writeBlock(DiskDriver* disk, void* src, int block_num) ***\n");
		
		char* in = "pippo";
		void* src = (void*) in;

		printf("\nWriting data = %s in block %d\n", in, block_num);
		int ret = DiskDriver_writeBlock(disk, src, block_num);
		printf("Function returned %d, data successfully written in block %d\n", ret, block_num);

		void* dest = (void*) malloc(BLOCK_SIZE);
		printf("\nRetrieving data from block %d\n", block_num);
		ret = DiskDriver_readBlock(disk, dest, block_num);
		printf("Function returned %d, data = %s\n", ret, (char*)dest);

		printf("\nWriting data in block %d\n", -1);
		ret = DiskDriver_writeBlock(disk, src, -1);
		printf("Function returned %d, data not written in block %d\n", ret, -1);

		printf("\nWriting data = paperino in block %d\n", block_num+3);
		ret = DiskDriver_writeBlock(disk, "paperino", block_num+3);
		printf("Function returned %d, data successfully written in block %d\n", ret, block_num+3);
		
		printf("\nRetrieving data from block %d\n", block_num+3);
		ret = DiskDriver_readBlock(disk, dest, block_num+3);
		printf("Function returned %d, data = %s\n", ret, (char*)dest);
  
		//~ DiskDriver_getFreeBlock(DiskDriver* disk, int start)
		//~ DiskDriver_freeBlock(DiskDriver* disk, int block_num)
		printf("\n*** Testing DiskDriver_getFreeBlock(DiskDriver* disk, int start) ***\n");
		printf("\n*** Testing DiskDriver_freeBlock(DiskDriver* disk, int block_num) ***\n");
		
		block_num = DiskDriver_getFreeBlock(disk, 0);
		printf("\nCurrent first free block = %d\n", block_num);
		
		ret = DiskDriver_freeBlock(disk, 0);
		printf("\nBlock %d freed with return value = %d\n", 0, ret);
		
		ret = DiskDriver_freeBlock(disk, block_num+3);
		printf("\nBlock %d freed with return value = %d\n", block_num, ret);
		
		block_num = DiskDriver_getFreeBlock(disk, 0);
		printf("\nCurrent first free block = %d\n", block_num);
			
		printf("\nMajor error checking\n");
		ret = 0;
		ret += DiskDriver_freeBlock(disk, block_num);                          //Tries freeing an empty block
		ret += DiskDriver_freeBlock(disk, block_num+BLOCKS);                   //Tries freeing an out of range block
		ret += DiskDriver_readBlock(disk, dest, block_num-1);                  //Tries reading an empty block
		ret += DiskDriver_readBlock(disk, dest, block_num+BLOCKS);             //Tries reading from an out of range block
		ret += DiskDriver_getFreeBlock(disk, block_num+BLOCKS);                //Tries retrieving a free block from out of range
		ret += DiskDriver_writeBlock(disk, src, block_num);                    //Writing on a block for next test
		ret += DiskDriver_writeBlock(disk, src, block_num);                    //Tries writing in a non free block
		ret += DiskDriver_writeBlock(disk, src, block_num+BLOCKS);             //Tries writing in an out of range block
		
		printf("\nDestroying disk driver\n");
		free(dest);
		DiskDriver_destroy(disk);
		
	}
	//FILE SYSTEM TEST
	else if(strcmp(test, "simplefs") == 0){
		printf("SIMPLE FILE SYSTEM TEST\n");
		
		// Test SimpleFS_init
		printf("\n+++ Test SimpleFS_init()");
		printf("\n+++ Test SimpleFS_format()\n");
		SimpleFS* fs = (SimpleFS*) malloc(sizeof(SimpleFS));
		DiskDriver* disk = (DiskDriver*) malloc(sizeof(DiskDriver));
		
		DiskDriver_init(disk, TEST_PATH, BLOCKS); 
	
		DirectoryHandle * directory_handle = SimpleFS_init(fs, disk);
		
		if(directory_handle != NULL) {
			printf("\n    File System creato e inizializzato correttamente\n");
		}else{
			printf("\n    Errore nella creazione del file system\n");
			return 0;
		}
		
		
		// Test SimpleFS_createFile
		printf("\n\n+++ Test SimpleFS_createFile()");
		int i, ret, num_file = 5;
		FileHandle* fl = malloc(sizeof(FileHandle));
		
		for(i = 0; i < num_file; i++) {
			char filename[128];
			sprintf(filename, "prova_%d.txt", directory_handle->dcb->num_entries);
			fl = SimpleFS_createFile(directory_handle,filename);
			if(fl != NULL) {
			printf("\n    File %s creato correttamente\n", filename);
			
			}
			else{
				printf("\n    Errore nella creazione di %s\n", filename);
				return 0;
			}
		}
			
				
		fl = SimpleFS_createFile(directory_handle,"prova_4.txt");
		if(fl != NULL) {
			printf("\n    File %s creato correttamente\n", "prova_4.txt");
			
		}else{
			printf("\n    Errore nella creazione di %s\n", "prova_4.txt");
		}
	
		
		
		// Test SimpleFS_openFile
		printf("\n\n+++ Test SimpleFS_openFile(directory_handle, prova_2.txt)");
		char nome_file[128] = "prova_2.txt";
		FileHandle * file_handle = malloc(sizeof(FileHandle));
		file_handle = SimpleFS_openFile(directory_handle, nome_file);
		ret = file_handle == NULL ? -1 : 0;
		
		if(file_handle != NULL) {
			printf("\n    File aperto correttamente\n");
		}else{
			printf("\n    Errore nell'apertura del file\n");
			
		}
		printf("\n    SimpleFS_openFile(directory_handle, \"%s\") => %d", file_handle->fcb->fcb.name, ret);
		
		
		
		

	 	// Test SimpleFS_mkDir
		printf("\n\n+++ Test SimpleFS_mkDir()");
		ret = SimpleFS_mkDir(directory_handle, "pluto");
		printf("\n    SimpleFS_mkDir(dh, \"pluto\") => %d", ret);
		if(ret == 0) {
			printf("\n    Cartella creata correttamente\n");
		}else{
			printf("\n    Errore nella creazione della cartella\n");
		}


	 	// Test SimpleFS_readDir
		printf("\n\n+++ Test SimpleFS_readDir()");
		printf("\n    Nella cartella ci sono %d elementi:\n", directory_handle->dcb->num_entries);
		char ** elenco2 = (char**) malloc(sizeof(char*)*directory_handle->dcb->num_entries);
		for(i = 0; i < directory_handle->dcb->num_entries; i++) {
			elenco2[i] = (char *) malloc(125);
		}
		SimpleFS_readDir(elenco2, directory_handle);
		for(i = 0; i < directory_handle->dcb->num_entries; i++) {
			printf("\n    > %s\n", elenco2[i]);
		}
		free(elenco2);

	 	// Test SimpleFS_openFile
		printf("\n\n+++ Test SimpleFS_openFile(directory_handle, prova_1.txt)");
		strcpy(nome_file, "prova_1.txt");
		file_handle = malloc(sizeof(FileHandle));
		file_handle = SimpleFS_openFile(directory_handle, nome_file);
		ret = file_handle == NULL ? -1 : 0;
		printf("\n    SimpleFS_openFile(directory_handle, \"%s\") => %d", file_handle->fcb->fcb.name, ret);
		if(file_handle != NULL) {
			printf("\n    File aperto correttamente\n");
		}else{
			printf("\n    Errore nell'apertura del file\n");
			return 0;
		}
		
		
		// Test SimpleFS_changeDir
		printf("\n\n+++ Test SimpleFS_changeDir()");
		printf("\n  curr_dir %s, ", directory_handle->dcb->fcb.name);
		printf("SimpleFS_changeDir(directory_handle, \"..\") ret=> %d", SimpleFS_changeDir(directory_handle, ".."));
		printf(", now in %s \n", directory_handle->dcb->fcb.name);
		
		printf("\n  curr_dir %s, ", directory_handle->dcb->fcb.name);
		printf("SimpleFS_changeDir(directory_handle, \"pluto\") ret=> %d", SimpleFS_changeDir(directory_handle, "pluto"));
		printf(", now in %s \n", directory_handle->dcb->fcb.name);
		
		printf("\n  curr_dir %s, ", directory_handle->dcb->fcb.name);
		printf("SimpleFS_changeDir(directory_handle, \"..\") ret=> %d", SimpleFS_changeDir(directory_handle, ".."));
		printf(", now in %s \n", directory_handle->dcb->fcb.name);
		
		printf("\n  curr_dir %s, ", directory_handle->dcb->fcb.name);
		printf("SimpleFS_changeDir(directory_handle, \"..\") ret=> %d", SimpleFS_changeDir(directory_handle, ".."));
		printf(", now in %s \n", directory_handle->dcb->fcb.name);
		
	 
		
		
		file_handle = SimpleFS_openFile(directory_handle, "prova_4.txt");
		char* string = "Sora, Donald and GoofySora, Donald and GoofySora, Donald and GoofySora, Donald and GoofySora, Donald and GoofySora, Donald and GoofySora, Donald and GoofySora, Donald and GoofySora, Donald and GoofySora, Donald and GoofySora, Donald and GoofySora, Donald and GoofySora, Donald and GoofySora, Donald and GoofySora, Donald and GoofySora, Donald and GoofySora, Donald and GoofySora, Donald and GoofySora, Donald and GoofySora, Donald and GoofyAO";
		
		printf("\n  Spazio primo blocco = %ld, mentre string = %ld \n", sizeof(file_handle->fcb->data), strlen(string));
				
		// Test write
		ret = SimpleFS_write(file_handle, string, strlen(string));
		printf("\n    %d bytes scritti\n", ret);
		printf("\n    %d size in bytes\n", file_handle->fcb->fcb.size_in_bytes);
		printf("\n    %d blocchi scritti\n", file_handle->fcb->fcb.size_in_blocks);
		
		int length = strlen(string);
		
		// Test SimpleFS_seek
		printf("\n\n+++ Test SimpleFS_seek()");
		int pos = 390;
		ret = SimpleFS_seek(file_handle, pos);
		printf("\n    SimpleFS_seek(file_handle, %d) => %d", pos, ret);
		if(ret == pos) {
			printf("\n    Spostamento del cursore avvenuto correttamente\n");
		}else{
			printf("\n    Errore nello spostamento del cursore\n");
			return 0;
		}
		
		ret = SimpleFS_write(file_handle, string, length);
		printf("\n    %d bytes scritti\n", ret);
		printf("\n    %d blocchi scritti\n", file_handle->fcb->fcb.size_in_blocks);
		
		
	 	// Test SimpleFS_read
		printf("\n\n+++ Test SimpleFS_read()");
		int size = file_handle->fcb->fcb.size_in_bytes;
		char data[size];
		printf("\n    SimpleFS_read(file_handle, data, %d) ha restituito: %d", size, SimpleFS_read(file_handle, (void*)data, size));
		printf("\n    Adesso \"data\" contiene: %s\n", data);

		
		
		
		
		
		printf("\n  curr_dir %s, ", directory_handle->dcb->fcb.name);
		printf("SimpleFS_changeDir(directory_handle, \"pluto\") ret=> %d", SimpleFS_changeDir(directory_handle, "pluto"));
		printf(", now in %s \n", directory_handle->dcb->fcb.name);
		
		for(i = 0; i < 5; i++) {
			char filename[128];
			sprintf(filename, "prova_%d.txt", i);
			fl = SimpleFS_createFile(directory_handle,filename);
			if(fl != NULL) {
			printf("\n    File %s creato correttamente\n", filename);
			
			}
			else{
				printf("\n    Errore nella creazione di %s\n", filename);
				return 0;
			}
		}
		
		// Test SimpleFS_readDir
		printf("\n\n+++ Test SimpleFS_readDir()");
		printf("\n    Nella cartella ci sono %d elementi:\n", directory_handle->dcb->num_entries);
		elenco2 = malloc(directory_handle->dcb->num_entries * 128);
		for(i = 0; i < directory_handle->dcb->num_entries; i++) {
			elenco2[i] = (char *) malloc(128);
		}
		SimpleFS_readDir(elenco2, directory_handle);
		for(i = 0; i < directory_handle->dcb->num_entries; i++) {
			printf("\n    > %s\n", elenco2[i]);
		}
		free(elenco2);
		
		printf("\n  curr_dir %s, ", directory_handle->dcb->fcb.name);
		printf("SimpleFS_changeDir(directory_handle, \"..\") ret=> %d", SimpleFS_changeDir(directory_handle, ".."));
		printf(", now in %s \n", directory_handle->dcb->fcb.name);
		
		
		// Test SimpleFS_readDir
		printf("\n\n+++ Test SimpleFS_readDir()");
		printf("\n    Nella cartella ci sono %d elementi:\n", directory_handle->dcb->num_entries);
		elenco2 = malloc(directory_handle->dcb->num_entries * 128);
		for(i = 0; i < directory_handle->dcb->num_entries; i++) {
			elenco2[i] = (char *) malloc(128);
		}
		SimpleFS_readDir(elenco2, directory_handle);
		for(i = 0; i < directory_handle->dcb->num_entries; i++) {
			printf("\n    > %s\n", elenco2[i]);
		}
		free(elenco2);
		
		
		// Test SimpleFS_remove		
		printf("\nnumber of free blocks %d\n", directory_handle->sfs->disk->header->free_blocks);
		
		printf("\n\n+++ Test SimpleFS_remove() [file]");
		strcpy(nome_file, "prova_1.txt");
		ret = SimpleFS_remove(directory_handle, nome_file);
		printf("\n    SimpleFS_remove(directory_handle, \"%s\") => %d", nome_file, ret);
		if(ret >= 0) {
			printf("\n    Cancellazione del file avvenuta correttamente\n");
		}else{
			printf("\n    Errore nella cancellazione del file\n");
		}
		printf("\nnumber of free blocks now %d\n", directory_handle->sfs->disk->header->free_blocks);
		
		
		// Test SimpleFS_readDir
		printf("\n\n+++ Test SimpleFS_readDir()");
		printf("\n    Nella cartella ci sono %d elementi:\n", directory_handle->dcb->num_entries);
		elenco2 = (char**) malloc(sizeof(char*)*directory_handle->dcb->num_entries);
		for(i = 0; i < directory_handle->dcb->num_entries; i++) {
			elenco2[i] = (char *) malloc(125);
		}
		SimpleFS_readDir(elenco2, directory_handle);
		for(i = 0; i < directory_handle->dcb->num_entries; i++) {
			printf("\n    > %s\n", elenco2[i]);
		}
		for(i = 0; i < directory_handle->dcb->num_entries; i++) {
			free(elenco2[i]);
		}
		free(elenco2);
		
		
		printf("\nnumber of free blocks %d\n", directory_handle->sfs->disk->header->free_blocks);
		printf("\n\n+++ Test SimpleFS_remove() [cartella]");
		strcpy(nome_file, "pluto");
		ret = SimpleFS_remove(directory_handle, nome_file);
		printf("\n    SimpleFS_remove(directory_handle, \"%s\") => %d", nome_file, ret);
		if(ret >= 0) {
			printf("\n    Cancellazione del file avvenuta correttamente\n");
		}else{
			printf("\n    Errore nella cancellazione del file\n");
			return 0;
		}
		
		printf("\nnumber of free blocks now %d\n", directory_handle->sfs->disk->header->free_blocks);
		
		// Test SimpleFS_readDir
		printf("\n\n+++ Test SimpleFS_readDir()");
		printf("\n    Nella cartella ci sono %d elementi:\n", directory_handle->dcb->num_entries);
		elenco2 = malloc(directory_handle->dcb->num_entries * 128);
		for(i = 0; i < directory_handle->dcb->num_entries; i++) {
			elenco2[i] = (char *) malloc(128);
		}
		SimpleFS_readDir(elenco2, directory_handle);
		for(i = 0; i < directory_handle->dcb->num_entries; i++) {
			printf("\n    > %s\n", elenco2[i]);
		}
		free(elenco2);
		
		SimpleFS_close(fl);
		free(directory_handle);
		DiskDriver_destroy(disk);
		free(fs);
	}
	else{
		printf("\033[0;31m"); 
		printf("\nUsage: ./simplefs_test.c <code>\n\n");
		printf("\033[0m"); 
		printf("If you want to test the bitmap module's functions: code = bitmap\n\n");
		printf("If you want to test the disk driver module's functions: code = disk_driver\n\n");
		printf("If you want to test the file system: code = simplefs\n\n");
		return 0;
	}
	
	
	
	
	
  //~ printf("FirstBlock size %ld\n", sizeof(FirstFileBlock));
  //~ printf("DataBlock size %ld\n", sizeof(FileBlock));
  //~ printf("FirstDirectoryBlock size %ld\n", sizeof(FirstDirectoryBlock));
  //~ printf("DirectoryBlock size %ld\n", sizeof(DirectoryBlock));
  
}
