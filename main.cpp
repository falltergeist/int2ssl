#include "stdafx.h"
#include "FalloutScript.h"
#include "XGetopt.h"
#include <iostream>

using namespace std;

// Globals
BOOL g_bDump = FALSE;
int g_nFalloutVersion = 2;
CString g_strIndentFill("\t");
BOOL g_bIgnoreWrongNumOfArgs = FALSE;
BOOL g_bInsOmittedArgsBackward = FALSE;
BOOL g_bStopOnError = FALSE;

CString g_strInputFileName;
CString g_strOutputFileName;


// Functions
void PrintUsage(char* lpszFileName);
BOOL ProcessCommandLine(int argc, char* argv[]);

int main(int argc, char* argv[])
{ 
    int nRetCode = 0;


    printf("Fallout script decompiler, version 8.3.0 (sfall edition)\n");
    printf("Copyright (C) Anchorite (TeamX), 2005-2009\n");
    printf("anchorite2001@yandex.ru\n");
    printf("Continued by Nirran, phobos2077 (2014-2015)\n");
    printf("Usage: int2ssl.exe [options] [-s value] file.int [file.ssl]\n");
    printf("Example: int2ssl.exe -d-1-a-b-e-s3 random.int\n");
    printf("  -d: dump file\n");
    printf("  -1: input file is Fallout 1 script\n");
    printf("  -a: ignore wrong number of arguments\n");
    printf("  -b: insert omitted arguments backward\n");
    printf("  -s: use Space instead of tab to indent\n");
    printf("  -e: stop decompiling on error\n");
    printf("  --: end of options\n");
    printf("\n");

//    if (argc < 2)
//    {
//        PrintUsage(argv[0]);
//        cin.get();
//        return 1;
//    }

//    if (!ProcessCommandLine(argc, argv))
//    {
//      return 1;
//    }

    CFile fileInput;
    CFile fileOutput;

    g_strInputFileName = "BECA.int";
    g_strOutputFileName = "BECA.int.ssl";

    if (!fileInput.Open(g_strInputFileName, CFile::modeRead | CFile::shareDenyWrite))
    {
        printf("Error: Unable open input file %s.\n", g_strInputFileName.c_str());
        if (g_bStopOnError == TRUE)
        {
            printf("Press ENTER to continue.\n");
            cin.get();
        }
        return 1;
    }

    if (!fileOutput.Open(g_strOutputFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText | CFile::shareDenyWrite))
    {
        printf("Error: Unable open output file %s.\n", g_strOutputFileName.c_str());
        if (g_bStopOnError == TRUE)
        {
            printf("Press ENTER to continue.\n");
            cin.get();
        }
        return 1;
    }

    try
    {
        CArchive arInput(&fileInput, CArchive::load);
        CFalloutScript Script;

        printf("Loading file %s...\n", g_strInputFileName.c_str());
        Script.Serialize(arInput);

        printf("File %s loaded successfuly\n", g_strInputFileName.c_str());
        printf("\n");

        CArchive arOutput(&fileOutput, CArchive::store);

        if (g_bDump)
        {
            printf("Dumping...\n", g_strInputFileName.c_str());
            Script.Dump(arOutput);

            printf("File %s dumped successfuly\n", g_strInputFileName.c_str());
        }
        else
        {
            printf("Decompiling...\n");

            printf("  Init definitions\n");
            Script.InitDefinitions();

            printf("  Processing code\n");
            Script.ProcessCode();

            printf("  Storing sources\n");
            Script.StoreSource(arOutput);
            printf("File %s decompiled successfuly\n", g_strInputFileName.c_str());
        }
    }

    catch (const UserException& e)
    {
        // Error message already displayed
        if (g_bStopOnError == TRUE)
        {
            printf("Press ENTER to continue.\n");
            cin.get();
        }
        return 1;
    }

    catch(...)
    {
        return 1;
        
//        const int c_nErrMsgLen = 1024;
//        TCHAR lpszErrMsg[c_nErrMsgLen];
//        e->GetErrorMessage(lpszErrMsg, c_nErrMsgLen);
//        printf("Error: %s.\n", lpszErrMsg);
//        if (g_bStopOnError == TRUE)
//        {
//            printf("Press ENTER to continue.\n");
//            cin.get();
//        }
//        return 1;
    }

    return 0;
}

void PrintUsage(char* lpszFileName)
{
    printf("Usage: int2ssl.exe [options] [-s value] file.int [file.ssl]\n");
    printf("Example: int2ssl.exe -d-1-a-b-e-s3 random.int\n");
    printf("\n");
    printf("Options\n");
    printf("  -d: dump file\n");
    printf("  -1: input file is Fallout 1 script\n");
    printf("  -a: ignore wrong number of arguments\n");
    printf("  -b: insert omitted arguments backward\n");
    printf("  -s: use Space instead of tab to indent\n");
    printf("  -e: stop decompiling on error\n");
    printf("  --: end of options\n");
}

BOOL ProcessCommandLine(int argc, char* argv[])
{
    int c;
    int nIndentWidth;

    while((c = getopt(argc, argv, "d1abes:")) != EOF)
    {
        switch (c)
        {
            case 'd':
                g_bDump = TRUE;
                printf("dump file is on.\n");
                break;

            case '1':
                g_nFalloutVersion = 1;
                printf("int2ssl is using Fallout 1 code.\n");
                break;

            case 'a':
                g_bIgnoreWrongNumOfArgs = TRUE;
                printf("ignore wrong number of arguments is on.\n");
                break;

            case 'b':
                g_bInsOmittedArgsBackward = TRUE;
                printf("insert omitted arguments backward is on.\n");
                break;

            case 'e':
                g_bStopOnError = TRUE;
                printf("stop decompiling on error is on.\n");
                break;

            case 's':
                nIndentWidth = atoi(optarg);

                if (nIndentWidth <= 0)
                {
                    printf("Warning: Invalid indent width. Indent set 3\n");
                    nIndentWidth = 3;
                }

                g_strIndentFill = "";

                for(; nIndentWidth > 0; nIndentWidth--)
                {
                    g_strIndentFill += " ";
                }
                break;
        }
    }

    if (optind == argc)
    {
        printf("Error: Input file name omitted\n");
        if (g_bStopOnError == TRUE)
        {
            printf("Press ENTER to continue.\n");
            cin.get();
        }
        return FALSE;
    }

    g_strInputFileName = argv[optind];
    optind++;

    if (optind < argc)
    {
        g_strOutputFileName = argv[optind];
    }
    else
    {
        g_strOutputFileName = g_strInputFileName;

//        if (g_strOutputFileName.Right(4).MakeLower() == ".int")
//        {
//            g_strOutputFileName = g_strOutputFileName.Left(g_strOutputFileName.GetLength() - 4);
//        }

        if (g_bDump)
        {
            g_strOutputFileName += ".dump";
        }
        else
        {
               g_strOutputFileName += ".ssl";
        }
    }
    return TRUE;
}
