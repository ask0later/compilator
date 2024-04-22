#ifndef DOTTER__
#define DOTTER__

//===============================================================================
//! @file    Dotter.h
//! @brief Áèáëèîòåêà äëÿ dot.
//!
//! $copyright: (C) Danila Baygushev$ <br>       
//===============================================================================

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
                                  //#include "windows.h"

//===============================================================================
//! @defgroup System Ñèñòåìíàÿ
//! @defgroup Main   Ãëàâíàÿ
//===============================================================================

//==============================================================================
#include "DotDef.h"  //Auto-gen file. Creates autamatically.
//===============================================================================

//===============================================================================
//! @ingroup Main
//! @brief   Ñòðóêòóðà óçëà.
//===============================================================================
struct newDotNodeStyle
{
    char Shape     [30]; //!< Ôîðìà. Çíà÷åíèÿ:<br>
                         //!1. box            - "êîðîáêà"(ïðÿìîóãîëüíèê)<br>
                         //!2. ellipse        - ýëëèïñ<br>
                         //!3. point          - òî÷êà<br>
                         //!4. egg            - ÿéöî<br>
                         //!5. triangle       - òðåóãîëüíèê<br>
                         //!6. diamond        - ðîìá<br>
                         //!7. trapezium      - òðàïåöèÿ<br>
                         //!8. parallelogram  - ïàðàëëåëîãðàìì<br>
                         //!9. hexagon        - øåñòèóãîëüíèê<br>
                         //!10.octagon        - âîñüìèóãîëüíèê<br>
                         //!11.invtriangle    - ïåðåâžðíóòûé òðåóãîëüíèê<br>
                         //!12.invtrapezium   - ïåðåâžðíóòàÿ òðàïåöèÿ<br>
                         //!13.folder         - ïàïêà<br>
                         //!14.tab            - çàêëàäêà<br>
                         //!15.component      - äåòàëü<br>
                         //!15.box3d          - 3D "êîðîáêà"(ïðÿìîóãîëüíèê)<br>
                         //!15.house          - äîì<br>
                                    
    char Color     [30]; //!< Öâåò ñíàðóæè. Çíà÷åíèÿ:<br>
                         //!1.àíãëèéñêèå íàçâàíèÿ öâåòîâ (ñì. Dotguide.pdf)<br>
                         //!2.# è çà íåé êàê â html<br>
                         //!3.òðè ÷èñëà double = öâåò / 256<br>
                         
    char FillColor [30]; //!< Öâåò âíóòðè. Çíà÷åíèÿ:<br>
                         //!1.àíãëèéñêèå íàçâàíèÿ öâåòîâ (ñì. Dotguide.pdf)<br>
                         //!2.# è çà íåé êàê â html<br>
                         //!3.òðè ÷èñëà double = öâåò / 256<br>
                         
    char FontColor [30]; //!< Öâåò áóêâ. Çíà÷åíèÿ:<br>
                         //!1.àíãëèéñêèå íàçâàíèÿ öâåòîâ (ñì. Dotguide.pdf)<br>
                         //!2.# è çà íåé êàê â html<br>
                         //!3.òðè ÷èñëà double = öâåò / 256<br>
                         
    char Style     [30]; //!< Ñòèëü. Çíà÷åíèÿ: <br>
                         //!1.solid     - îáû÷íûé<br>
                         //!2.dashed    - ïóíêòèðíûé<br>
                         //!3.dotted    - òî÷êàìè<br>
                         //!4.bold      - æèðíûé<br>
                         //!5.invis     - íå ðèñóåòñÿ<br>                         
                         //!6.filled    - çàïîëíåí<br>
                         //!7.diagonals - äèàãîíàëè<br>
                         //!8.round     - ñêðóãëžíííûå óãëû<br>
    
    newDotNodeStyle ();
    
    newDotNodeStyle& shape     (const char Value[]);
    newDotNodeStyle& color     (const char Value[]);
    newDotNodeStyle& fillcolor (const char Value[]);
    newDotNodeStyle& fontcolor (const char Value[]);
    newDotNodeStyle& style     (const char Value[]);
};

//------------------------------------------------------------------------------

newDotNodeStyle::newDotNodeStyle ()
{
    strcpy (Shape,     "ellipse");
    strcpy (Color,     "black");
    strcpy (FillColor, "black");
    strcpy (FontColor, "black");
    strcpy (Style,     "solid");    
}

newDotNodeStyle& newDotNodeStyle::shape     (const char Value[])
{
    strcpy (Shape, Value);
    
    return *this;
}
newDotNodeStyle& newDotNodeStyle::color     (const char Value[])
{
    strcpy (Color, Value);
    
    return *this;
}
newDotNodeStyle& newDotNodeStyle::fillcolor (const char Value[])
{
    strcpy (FillColor, Value);
    
    return *this;
}
newDotNodeStyle& newDotNodeStyle::fontcolor (const char Value[])
{
    strcpy (FontColor, Value);
    
    return *this;
}
newDotNodeStyle& newDotNodeStyle::style     (const char Value[])
{
    strcpy (Style, Value);
    
    return *this;
}

//==============================================================================

//===============================================================================
//! @ingroup Main
//! @brief   Ñòðóêòóðà ñâÿçè.
//===============================================================================
struct newDotLinkStyle
{
    char Style [30]; //!< Ñòèëü. Çíà÷åíèÿ: <br>
                     //!1.solid     - îáû÷íûé<br>
                     //!2.dashed    - ïóíêòèðíûé<br>
                     //!3.dotted    - òî÷êàìè<br>
                     //!4.bold      - æèðíûé<br>
                     //!5.invis     - íå ðèñóåòñÿ<br>
                   
    char Color [30]; //!< Öâåò ëèíèè. Çíà÷åíèÿ:<br>
                     //!1.àíãëèéñêèå íàçâàíèÿ öâåòîâ (ñì. Dotguide.pdf)<br>
                     //!2.# è çà íåé êàê â html<br>
                     //!3.òðè ÷èñëà double = öâåò / 256<br>
                        
    newDotLinkStyle ();
    
    newDotLinkStyle& style (const char Value[]);
    newDotLinkStyle& color (const char Value[]);
};

//------------------------------------------------------------------------------

newDotLinkStyle::newDotLinkStyle ()
{
    strcpy (Color, "black");
    strcpy (Style, "solid");
}

newDotLinkStyle& newDotLinkStyle::style (const char Value[])
{
    strcpy (Style, Value);
    
    return *this;
}
newDotLinkStyle& newDotLinkStyle::color (const char Value[])
{
    strcpy (Color, Value);
    
    return *this;
}

//==============================================================================

//===============================================================================
//! @ingroup System
//! @brief   Çäåñü õðàíÿòñÿ íåêîòîðûå ñèñòåìíûå äàííûå.
//===============================================================================
namespace DotterSystem
{      
    FILE* CurrentFile = NULL; //!< Òåêóùèé ôàéë     
    bool CloseAtEnd = false; //!< Çàêðûâàòü ôàéë ïîñëå êîíöà, èëè íåò
          
    newDotNodeStyle DefDotNode; //!< Ñòàíäàðòíûé ñòèëü óçëà.
    newDotLinkStyle DefDotLink; //!< Ñòàíäàðòíûé ñòèëü ññûëêè.  
    
    newDotNodeStyle CurrentNodeStyle; //!< Òåêóùèé ñòèëü óçëà.
    newDotLinkStyle CurrentLinkStyle; //!< Òåêóùèé ñòèëü ññûëêè.  
    
    int Ind = 4; //!< Îòñòóï ïåðåä ñòðîêîé â êîë-âå ïðîáåëîâ.            
};
using namespace DotterSystem;

//==============================================================================

//===============================================================================
//! @ingroup System
//! @brief   Ôóíêöèÿ âûâîäà îøèáêè.
//!
//! @param Message - ñîîáùåíèå, ïîÿñíÿþùåå îøèáêó
//===============================================================================
static void dtError (const char Message []);

//==============================================================================

//===============================================================================
//! @ingroup Main
//! @brief   Íà÷àòü çàïèñü dot ãðàôà.
//!
//! @param To - Ôàéë, êóäà ïèñàòü. Âñå êîìàíäû (ñì. @ref dtNode, @ref dtLink) äî @ref dtEnd áóäóò ïèñàòüñÿ â ýòîò ôàéë
//===============================================================================
static void dtBegin (FILE* To);

//===============================================================================
//! @ingroup Main
//! @brief   Îòêðûòü ôàéë è íà÷àòü çàïèñü dot ãðàôà.
//!
//! @param Name - Èìÿ ôàéëà, êóäà ïèñàòü. Âñå êîìàíäû (ñì. @ref dtNode, @ref dtLink) äî @ref dtEnd áóäóò ïèñàòüñÿ â ýòîò ôàéë
//===============================================================================
static inline void dtBegin (const char Name []);

//===============================================================================
//! @ingroup Main
//! @brief   Çàêîí÷èòü îïèñàíèå dot ãðàôà.
//===============================================================================
static void dtEnd  ();

//===============================================================================
//! @ingroup Main
//! @brief   Âûâåñòè ñòðîêó â ïðèãîäíîì äëÿ Dot ôîðìàòå.
//===============================================================================
static void dtOut (const char Str []);

//==============================================================================

//===============================================================================
//! @ingroup Main
//! @brief   Óñòàíàâëèâàåò òåêóùèé ñòèëü óçëà.
//!
//! @param NodeStyle - Ñòðóêòóðà, ñ îïèñàííûì ñòèëåì óçëà. Íå îáÿçàòåëüíî
//!
//! @return Ññûëêà íà òåêóùèé ñòèëü. Ïîçâîëÿåò ïèñàòü òàê:
//!
//! @usage
//! @code
//! 
//!   dtNodeStyle ().shape ("diamond")
//!                 .color ("red");
//!
//! @endcode     
//===============================================================================
newDotNodeStyle& dtNodeStyle  (const newDotNodeStyle NodeStyle = CurrentNodeStyle);

//===============================================================================
//! @ingroup Main
//! @brief   Óñòàíàâëèâàåò òåêóùèé ñòèëü óçëà.
//!
//! @param LinkStyle - Ñòðóêòóðà, ñ îïèñàííûì ñòèëåì óçëà. Íå îáÿçàòåëüíî
//!
//! @return Ññûëêà íà òåêóùèé ñòèëü. Ïîçâîëÿåò ïèñàòü òàê:
//!
//! @usage
//! @code
//! 
//!   dtLinkStyle ().style ("bold")
//!                 .color ("red");
//!
//! @endcode   
//===============================================================================
newDotLinkStyle& dtLinkStyle  (const newDotLinkStyle LinkStyle = CurrentLinkStyle);

//===============================================================================
//! @ingroup Main
//! @brief   Ôóíêöèÿ ðèñîâàíèÿ óçëà â dot.
//!
//! @param NodeN       - Íîìåð óçëà
//! @param Label       - Òåêñò, êîòîðûé íàäî ðàçìåñòèòü âíóòðè
//! @param NodeStyle   - Ñòðóêòóðà, ñ îïèñàííûì ñòèëåì óçëà. Åñëè íå óêàçàíî, èñïîëüçóåòñÿ òåêóùàÿ(ñì. @ref dtNodeStyle)
//! @param ExtraParams - Äîïîëíèòåëüíûå ïàðàìåòðû(ñì. dotguide)
//===============================================================================
static void dtNode  (const int NodeN, const char Label [] = "", const newDotNodeStyle NodeStyle = CurrentNodeStyle, const char ExtraParams [] = "");

//===============================================================================
//! @ingroup Main
//! @brief   Ôóíêöèÿ ðèñîâàíèÿ ñîåäèíåíèÿ óçëîâ â dot.
//!
//! @param FromNode    - Íîìåð óçëà îòêóäà
//! @param ToNode      - Íîìåð óçëà êóäà
//! @param Label       - Òåêñò, êîòîðûé íàäî ðàçìåñòèòü ðÿäîì.
//! @param LinkStyle   - Ñòðóêòóðà, ñî ñòèëåì ñâÿçè. Åñëè íå óêàçàíî, èñïîëüçóåòñÿ òåêóùèé (ñì. @ref dtLinkStyle)
//! @param ExtraParams - Äîïîëíèòåëüíûå ïàðàìåòðû(ñì. dotguide)
//===============================================================================
static void dtLink (const int FromNode, const int ToNode, const char Label [] = "", const newDotLinkStyle LinkStyle = CurrentLinkStyle, const char ExtraParams [] = "");

//==============================================================================

//===============================================================================
//! @ingroup Main
//! @brief   Ôóíêöèÿ ãåíåðèðîâàíèÿ èçîáðàæåíèÿ â dot.
//!
//! @param From - Èìÿ dot ôàéëà
//! @param To   - Èìÿ âûõîäíîãî ôàéëà 
//! @param Type - Òèï (jpg, bmp, ...)
//! @param Show - Ïîêàçûâàòü ôàéë ïîñëå ñîçäàíèÿ, èëè íåò
//===============================================================================
static void dtRender (const char From [], const char To [] = "Graph.jpg", const char Type [] = "jpg", const bool Show = true);

//==============================================================================

static void dtError (const char Message [])
{
    throw Message;    

    printf ("==Dotter error!==\n");
    //printf ("Message = %s\n");
    
    throw "Dotter error!!!"; //Âäðóã êòî-òî ëîâèò. ) 
    
    system ("pause");
  
    exit (0);
}

//==============================================================================

static void dtBegin (FILE* To)
{ 
    if (!To) dtError ("No output file");
    if (CurrentFile) dtError ("Close previous file first");      
    CurrentFile = To;
    
    fprintf (CurrentFile, "digraph DotGraph\n");
    fprintf (CurrentFile, "{\n");
    //fprintf (CurrentFile, "rankdir=LR;\n");
}

static inline void dtBegin (const char Name [])
{
    FILE* To = NULL;
    To = fopen (Name, "w");
    
    CloseAtEnd = true;
    
    dtBegin (To);       
}

static void dtEnd()
{
    if (!CurrentFile) dtError ("No output file");      
    
    fprintf (CurrentFile, "}\n");     
    
    if (CloseAtEnd)
    {
        CloseAtEnd = false;
        
        fclose (CurrentFile);               
    }
    
    CurrentFile = NULL;
}

static void dtOut (const char Str [])
{
    if (!CurrentFile) dtError ("No output file");       
     
    for (int i = 0; Str [i] != 0; i ++)
    {
        switch (Str [i])
        {
            case '\n':
            {
                fprintf (CurrentFile, "\\n");    
                break;        
            }
            // case '\r':
            // {
            //     fprintf (CurrentFile, "\\r");    
            //     break;        
            // }
            case '\\':
            {
                fprintf (CurrentFile, "\\\\");    
                break;        
            }
            case '"':
            {
                fprintf (CurrentFile, "\\\"");    
                break;        
            }
            
            default:
            {
                fprintf (CurrentFile, "%c", Str [i]);    
                break;        
            }        
                        
        }    
    }     
}

//==============================================================================

newDotNodeStyle& dtNodeStyle  (const newDotNodeStyle NodeStyle)
{
    if (!CurrentFile) dtError ("No output file");      
    
    CurrentNodeStyle = NodeStyle;
    
    return CurrentNodeStyle;
}
newDotLinkStyle& dtLinkStyle  (const newDotLinkStyle LinkStyle)
{
    if (!CurrentFile) dtError ("No output file");      
    
    CurrentLinkStyle = LinkStyle;
    
    return CurrentLinkStyle;     
}

void dtNode (const int NodeN, const char Label [], const newDotNodeStyle NodeStyle, const char ExtraParams [])
{
    if (!CurrentFile) dtError ("No output file");      
    
    for (int i = 0; i < DotterSystem::Ind; i ++)
    {
        fprintf (CurrentFile, " ");    
    }
    
    fprintf (CurrentFile, "Node%d ", NodeN);
    fprintf (CurrentFile, "[");
    
#ifndef BeautifulDot
    fprintf (CurrentFile, "shape=\"%s\", ",     NodeStyle.Shape);
    fprintf (CurrentFile, "color=\"%s\", ",     NodeStyle.Color);
    fprintf (CurrentFile, "fontcolor=\"%s\", ", NodeStyle.FontColor);
    fprintf (CurrentFile, "fillcolor=\"%s\", ", NodeStyle.FillColor);
    fprintf (CurrentFile, "style=\"%s\", ",     NodeStyle.Style);
    fprintf (CurrentFile, "weight=\"1\", ");
#else    
    if (strcmp (NodeStyle.Shape,     DefDotNode.Shape)     != 0) fprintf (CurrentFile, "shape=\"%s\", ",     NodeStyle.Shape);
    if (strcmp (NodeStyle.Color,     DefDotNode.Color)     != 0) fprintf (CurrentFile, "color=\"%s\", ",     NodeStyle.Color);
    if (strcmp (NodeStyle.FontColor, DefDotNode.FontColor) != 0) fprintf (CurrentFile, "fontcolor=\"%s\", ", NodeStyle.FontColor);
    if (strcmp (NodeStyle.FillColor, DefDotNode.FillColor) != 0) fprintf (CurrentFile, "fillcolor=\"%s\", ", NodeStyle.FillColor);
    if (strcmp (NodeStyle.Style,     DefDotNode.Style)     != 0) fprintf (CurrentFile, "style=\"%s\", ",     NodeStyle.Style); 
#endif
    
    fprintf (CurrentFile, "label=\"");
    dtOut (Label);    
    fprintf (CurrentFile, "\"");

    if (strcmp (ExtraParams, "") != 0) fprintf (CurrentFile, ", %s", ExtraParams);
    
    fprintf (CurrentFile, "];\n");   
}

static void dtLink (const int FromNode, const int ToNode, const char Label [], const newDotLinkStyle LinkStyle, const char ExtraParams [])
{
    if (!CurrentFile) dtError ("No output file");      
    
    for (int i = 0; i < DotterSystem::Ind; i ++)
    {
        fprintf (CurrentFile, " ");    
    }
    
    fprintf (CurrentFile, "Node%d -> Node%d ", FromNode, ToNode);
    fprintf (CurrentFile, "[");

#ifndef BeautifulDot    
    fprintf (CurrentFile, "color=\"%s\", ", LinkStyle.Color);
    fprintf (CurrentFile, "style=\"%s\", ", LinkStyle.Style); 
    fprintf (CurrentFile, "weight=\"1\", ");     
#else    
    if (strcmp (LinkStyle.Color, DefDotLink.Color) != 0) fprintf (CurrentFile, "color=\"%s\", ", LinkStyle.Color);
    if (strcmp (LinkStyle.Style, DefDotLink.Style) != 0) fprintf (CurrentFile, "style=\"%s\", ", LinkStyle.Style); 
#endif
    
    fprintf (CurrentFile, "label=\"");
    dtOut (Label);    
    fprintf (CurrentFile, "\"");

    if (strcmp (ExtraParams, "") != 0) fprintf (CurrentFile, ", %s", ExtraParams);
    
    fprintf (CurrentFile, "];\n");   
}

//==============================================================================

static void dtRender (const char From [], const char To [], const char Type [], const bool Show)
{
    const int ComandSize = 100; 
     
    char Comand [ComandSize] = "";
    
    const int OtherSymbols = 14;
    
    if (strlen (__DOT__) + strlen (From) + strlen (To) + strlen (Type) + OtherSymbols >= ComandSize) dtError ("Names too long!");
    
    sprintf (Comand, "%s -T%s %s -o %s", __DOT__, Type, From, To);
    
    system (Comand);
    
    if (Show)
    {
        strcpy (Comand, "");
        
        sprintf (Comand, "start %s", To);
    
        system (Comand);     
    }     
}

//==============================================================================

#endif