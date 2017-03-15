###Background 
Some operating systems have addressed this by continuously indexing the names of files in the file system, and then providing the ability to query the index. Server systems generally do not continuously index file names, instead providing one or more applications for searching the file system for files whose names match the desired patterns. Linux provides the find application for general traversal of the file system. The syntax for the find application is extremely difficult to master. 

The goal for this assignment is to design and build a high-performance application that will recursively crawl through a set of specified directories, looking for filenames that match a specified bash pattern.
For very large software systems, a singly-threaded application to crawl the directories may take a very long time. The purpose of this assessed exercise is to develop a concurrent file system crawler in C, exploiting the concurrency features of PThreads, to rapidly, recursively crawl through the nominated directories, looking for filenames that match a specified pattern.

###SpecificationThis project is to create a file named fileCrawler.c. The program understands the following arguments:
> **patern** indicates a bash pattern that filenames match 

> **directory** directory to be recursively crawled for matching filenames 

The usage:  **./fileCrawler pattern [directory] ...**

When you type a command to bash, it splits what you have typed into words. It scans each word for the characters:
> ‘*’, ‘?’, and ‘[‘. 

If one of these characters appears, then the word is regarded as a pattern, and replaced with an alphabetically sorted list of filenames in the specified directorymatching the pattern. 

The fileCrawler will look for these patterns recursively in the specified directories. Note that when you specify pattern to fileCrawler, you will have to enclose it in single quotes to prevent bash from interpreting it
Any character that appears in a pattern, other than the special characters described below, matches itself.
The special pattern characters have the following meanings: 

```
*: 			Matches any string, including the null string.?:  		Matches any single character.[...]:  	Matches any one of the enclosed characters. 

```
