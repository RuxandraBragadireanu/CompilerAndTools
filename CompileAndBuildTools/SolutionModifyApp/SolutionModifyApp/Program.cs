using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Collections;
using System.IO;
using System.Diagnostics;

namespace SolutionModifyApp
{
    class Program
    {       
        static string projectInputFilename = "";
        static string projectOutputFilename = "";
        static XmlDocument doc = null;
        static XmlNamespaceManager nsMgr = null;

        static void AddAdditionalItemsTo(XmlDocument doc, XmlNamespaceManager mgr, string toNodeString, LinkedList<string> additionalItems)
        {
            IEnumerator nodeIter = doc.SelectNodes(toNodeString, mgr).GetEnumerator();
            while (nodeIter.MoveNext())
            {
                XmlNode node = nodeIter.Current as XmlNode;
                foreach (string addItem in additionalItems)
                {
                    // not efficient but doesn't matter
                    node.FirstChild.InnerText = addItem + ";" + node.FirstChild.InnerText;
                }
            }
        }

        static bool FillInputStrings(LinkedList<string> includeDirs, LinkedList<string> libDirs, LinkedList<string> libLinked, LinkedList<string> filesToAdd)
        {
            LinkedList<string>[] targets = new LinkedList<string>[5];
            targets[0] = null;
            targets[1] = includeDirs;
            targets[2] = libDirs;
            targets[3] = libLinked;
            targets[4] = filesToAdd;

            try
            {
                using (StreamReader sr = new StreamReader("Def.txt"))
                {
                    String line = sr.ReadLine();
                    
                    // Read all four groups of '%'. Ignore the first set of items.                 
                    for (int i = 0; i < 5; i++)
                    {
                        if (line[0] != '%') return false;
                        if (sr.EndOfStream)
                        {
                            if (i == 4) break;
                            else return false;
                        }

                        do
                        {
                            line = sr.ReadLine();
                            if (targets[i] != null && line[0] != '%')
                                targets[i].AddLast(line);
                        } while (line[0] != '%' && !sr.EndOfStream);
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine("Error: The file could not be read:");
                Console.WriteLine(e.Message);
            }

            return true;
        }

        static void ModifyIncludeAndLinkerDirAndFiles(LinkedList<string> includeDirs, LinkedList<string> libDirs, LinkedList<string> libLinked)
        {
            AddAdditionalItemsTo(doc, nsMgr, "/ns:Project/ns:PropertyGroup/ns:IncludePath", includeDirs);
            AddAdditionalItemsTo(doc, nsMgr, "/ns:Project/ns:PropertyGroup/ns:LibraryPath", libDirs);
            AddAdditionalItemsTo(doc, nsMgr, "/ns:Project/ns:ItemDefinitionGroup/ns:Link/ns:AdditionalDependencies", libLinked);

            doc.Save(projectOutputFilename);            
        }

        static void AppendFileToNode(XmlDocument doc, XmlNode node, string nodeName, string fileName)
        {
            XmlNode newNode = doc.CreateElement(nodeName, doc.DocumentElement.NamespaceURI);
            XmlAttribute attr = doc.CreateAttribute("Include");
            attr.Value = fileName;
            newNode.Attributes.Append(attr);
            node.AppendChild(newNode);
        }

        static void ModifyProjectSourceFiles(LinkedList<string> filesToAdd)
        {
            IEnumerator nodeIter = doc.SelectNodes("/ns:Project/ns:ItemGroup", nsMgr).GetEnumerator();
            while (nodeIter.MoveNext())
            {
                XmlNode node = nodeIter.Current as XmlNode;
                if (node.FirstChild.Name == "ClCompile")
                {
                    foreach (string addItem in filesToAdd)
                        AppendFileToNode(doc, node, "ClCompile", addItem);
                }
                else if (node.FirstChild.Name == "ClInclude")
                {
                    foreach (string addItem in filesToAdd)
                        AppendFileToNode(doc, node, "ClInclude", addItem);
                }
            }
        }

        static void Main(string[] args)
        {            
            LinkedList<string> includeDirs = new LinkedList<string>();
            LinkedList<string> libDirs = new LinkedList<string>();
            LinkedList<string> libLinked = new LinkedList<string>();
            LinkedList<string> filesToAdd = new LinkedList<string>();

            string slnDir = Environment.GetEnvironmentVariable("AGAPIAPATH");
            if (slnDir == null)
            {
                Console.WriteLine("CAN'T FIND AGAPIAPATH variable");
            }

            projectInputFilename = slnDir + "\\Compiler\\Compiler.vcxproj";
            projectOutputFilename = slnDir + "\\Compiler\\Compiler.vcxproj";

            /// Local test enable
            /// ---------------------------
            //string projectOutputFilename = slnDir + "\\Compiler\\Compiler_c.vcproj";
            // Copy file
            //System.IO.File.Copy(projectInputFilename, projectOutputFilename, true);
            //-----------------------------

             // Create xml document
            doc = new XmlDocument();
            doc.Load(projectInputFilename);
            nsMgr = new XmlNamespaceManager(doc.NameTable);
            nsMgr.AddNamespace("ns", "http://schemas.microsoft.com/developer/msbuild/2003");


            if (FillInputStrings(includeDirs, libDirs, libLinked, filesToAdd))
            {
                ModifyIncludeAndLinkerDirAndFiles(libDirs, libDirs, libLinked);
                ModifyProjectSourceFiles(filesToAdd);
            }
            else
            {
                Console.WriteLine("Error: There is a problem with the definition file!!");
                Debug.Assert(false);                      
            }

            // Save the xml doc
            doc.Save(projectOutputFilename);
        }
    }
}
