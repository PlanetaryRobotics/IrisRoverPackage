// Names and messages to be sent to the Moon.
// Post-processed to remove/substitute non-ASCII characters, have consistent formatting, and ensure no name/messages is longer than 99char (FW_LOG_STRING_MAX_SIZE).
#include "NamesAndMessages.hpp"

namespace IrisNames
{
    // NOTE: All names/messages are separated with null terminators here so this is basically just a bunch of consecutive cstrings in memory.

    // Longest Name: 38char, count = 796, exact duplicates removed
    unsigned int LEN_CREDITS = 11091;
    const char CREDITS[] =
        "Alec Probst\0Justin Nguyen\0Michael Roberts\0Griffin Della Grotte\0Tom Scherlis\0Thor Mercier\0Yin Zhong\0Jonathan Schwartz\0John Lore\0Hannah Tomio\0Mason Xiao\0Yonatan Nozik\0Nima Rahnemoon\0Lucas Moiseyev\0Tanvi Bhargava\0Suyash Bhatt\0Abhishek Bamotra\0Jonathan Ali\0Ryan Huang\0Jake Tesler\0Jon Schiavo\0Matt Zischkau\0Adam Quinn\0Geoffrey Cohen\0Jeremy Meola\0Andrew Zhang\0Jiaqi Liu\0Siddharth Girdhar\0Scott Krulcik\0Vignesh Rajmohan\0Shuwen Wang\0Martial Sawasawa\0Andrzej Jackowski\0Andrea Davis\0Connor Colombo\0Oleg Sapunkov\0William Ganucheau\0Nathan King\0Lawrence Papincak\0Curt Boirum\0Nick Acuna\0Meghan Cilento\0Hailey Moosbrugger\0Lou Chomas\0Antonio Song\0Ivy He\0Anna Fuller\0Alessandra Fleck\0Andrew Paul Tallaksen\0Divya Rao\0Tim Angert\0Nikolai Stefanov\0Jon Anderson\0Siri Maley\0Matt Schnur\0Andrew Horchler\0Cory Bird\0Red Whittaker\0Heather Jones\0Ben McKeeby\0Mike Provenzano\0Diana Zhang\0Nora Kazour\0Daniel Scher\0Paulo Fisch\0Jay Maier\0Vidhi Patel\0Joe Wright\0Eric Trimbur\0Evan Aronhalt\0Fernando Tabares\0Ignacio Cordova\0Julianne Sanscartier\0Elizabeth Armon\0Andrea Sipos\0Ceci Morales Garza\0Shreyas Shenoy\0Nick Crispie\0Akshay Ramkrishnan\0Danae Petroulakis\0Roberto Jaime\0Karthik Chandrashekaraiah\0Ryan O'Keefe\0Kushal Sandeep Doshi\0Paul Ramirez\0Kashyap Adury\0Joe Mertz\0Felipe Daniel Oropeza\0Ben Martin\0Ashu Sharma\0Praveen Vulimiri\0Inez Khan\0Ben Chaffee\0Sai Swaminathan\0Caleigh Goodwin-Schoen\0Erick Ramirez\0Johnathan Cassady\0Manan Ghelani\0Akash Garg\0Luke Tsai\0Kevin Low\0Julian DeLano\0Shrirang Patki\0Dimitrios Nikitopoulos\0Sandeep Mistry\0Stephanie Manasterski\0Leo Cervantes\0Victoria Han\0Andrew Ye\0Josh Wilson\0Rasmus Bergstrom\0Sharon Liu\0Jon Stapchuck\0Aaron McKenzie\0Manoj Bhat\0Alexander Baikovitz\0Jessica Meng\0Ian Holst\0Marissa DeFallo\0Nathan Kaleida\0Morgan Montalvo\0Joey Joseph\0Tomi Akin-Sodipo\0Kunal Ashok\0Abdulaziz Almusa\0Jasmine Bitanga\0Ray Galeza\0Lindy Cornwall\0Nishant Sharma\0Chuck Whittaker\0Sanjay Narayanan\0Hannah Lyness\0Youie Cho\0Aabharan Hemanth\0Qiang Gu\0Stephanie Tam\0CJ May\0Neil Khera\0Sid Sahoo\0Ben Boxer\0Ray Pai\0Daniel Glazer\0Nandita Kumari\0Mickey Velado\0Evan Evan\0Mike Chiang\0Eugene Fang\0Abi Kalpathi Suresh\0Jaghvi Mehta\0Philipp Schneider\0Ben Holden\0Max McHugh\0Vignan Velivela\0Mukundh Balajee\0Sora Shin\0Avi Padmanabhan\0Rafi Segal\0Adi Magal\0Jordan Ford\0Pat Callaghan\0Joe Bartels\0Jay Jasper\0Chris Cunningham\0Helen Song\0Jinglun Ding\0Michelle Chen\0Mia Tang\0Gabbi LaBorwit\0Urara Kono\0Ali Albazroun\0Bhakti Shah\0Sofia Hurtado\0Daniel Arnett\0Mike Gee\0Shannon Case\0Eddie Tapia\0Helen Tsui\0Vicky Zhou\0Harjatin Baweja\0Janelle Wen\0Lauren Hung\0Chloe SunMin Kim\0Varun Raghav Ramesh\0Hendrik Dreger\0Hannah Bartoshesky\0Lena Li\0Coy Conduff\0Dylan Hawley\0Ashley Taylor\0Cora Wang\0Annette Hong\0John R. Mann\0Riva Fouzdar\0Rohith Pillai\0Timmy Chiu\0Caitlin Coyiuto\0Estevan Chairez\0Rachel Lee\0Oskar Schlueb\0Sharon (Dayoung) Lee\0Xiaobin Li\0Rachel Fu\0Sun Min Kim\0Haley Park\0Jun Kim\0SungHo Hwang\0Siyi Cao\0Jay Huh\0Rai Munoz\0Evan Harber\0Danny Sepulveda\0Matt Hyatt\0Neha Gautam\0Leon Lu\0Yuan Chen\0Cameron Harper\0Jimin Lee\0Ricky Huang\0Yuqing Lin\0Zach Muraskin\0James Chen\0Tejas Venkatesh\0Hunter Wodzenski\0Madhav Gajula\0Alex Wittig\0Arielle Min\0Matt Davis\0Kevin Fang\0Abby Zhang\0Carmella Li\0Shawn Park\0Hari Prasath John Kennedy\0Jeffery John\0Peggy Shen\0Adithi Phadke\0beatriz holzbach\0Mariel Peczak\0Carmyn Talento\0Jonah Davis\0William Gay\0Ruth Peter\0Advait Wadhwani\0Shilin Ma\0Cheyu Tu\0Ines Rodriguez Hsu\0Lilly Das\0Gierad Laput\0Shuang Wu\0Kajal Mehta\0Stephen Mao\0Erin Isabel Anand\0Perrin Tong\0Parth Iyer\0Neeta Khanuja\0Tony Hwang\0Peiqi Liu\0Julian Rodriguez\0Grace Kumble\0Taruna Sudhakar\0Akhil Karra\0Harvin Park\0Harrison Chui\0Isha Kanodia\0Yee Kit Chan\0Jolyne Wang\0Abhinand Jha\0Diya Handa\0Tejas Sarvankar\0Yichuan Luo\0Sonja Michaluk\0Venkat Gella\0Swati Anshu\0Anya Jasthi\0Esha Mundra\0Mehar Goli\0Grace Lin\0Victoria Santiago\0Bobby Gower\0Matthew Hyatt\0Samuel Lapides\0Tianwei Li\0CMU Children\'s School\0Chris and Jennifer Urmson\0Dr. Anand Chandikadas Choudhari\0Alisa Pugacheva\0Sherry Juelfs\0Scott Juelfs\0Greg, Deirdre, & Carolyn Manasterski\0Tara & Matthew Manasterski\0Jordan Taylor\0Elizabeth Juelfs\0Sandra Goldman\0Guilherme Pereira\0Brett Sommers\0Kenzi Jones\0Ariana Nathani\0Lindsey Colombo\0Aditya Rao\0Robert Colombo\0Deepa Rao\0Arun Rao\0Dennis Crummey\0Serena Wang\0Tanmay Raj\0Aranka Barta\0Graham Neubig\0Jon Adams\0Christie Lagemann\0Heather Justice\0Oz Osborn\0Andre Kenneth Chase Randall\0Roy Riggs\0Terrence Toon\0Kelsea Langley\0Ryan Langley\0Gabriella Pleasant\0Murray Gudesblat\0Bill and Lori Whitley\0Emma Karolyi\0Artur Fabricio\0Christina Ou\0Kevin Ku\0Ian Miller\0Jing Chang\0Alex Daniels\0Jack Wardale\0Georgia Fowkes\0christina scholehwar\0Heather Curtis\0Devan Lippman\0Michelle Chou\0John Schoen\0Rachel Goodwin\0Zach Fagan\0Patrick Luvara\0Christina Pollet\0John Hrivnak\0Steven Kable\0Ashley Kidd\0Scott Moreland\0James McNulty\0David Taus\0David Simon\0Kevin Lynch\0Stella Angert\0Justine Olshan\0Doge Coin\0Alan D. Guisewite\0Lauren Schneider\0Alyce Colombo\0Dr. Robert Roy Colombo\0William V Dwyer Sr.\0Eileen Dwyer\0Brian Lipke\0Debbie Lipke\0Rosa Elena Murcia Coronado\0Prateek Tandon\0Thomas Bryan\0Sarjoun Skaff\0Nagendranath Maley\0Gabby Odusanya\0Roshan Gurmeet Singh\0Ali Yousafzai\0Elliot Popowitz\0Priyamvada Thushara Pillai\0Jayashree Panickar\0Thulasi Kumar M.K. Pillai\0Yung Kay Lo\0Halanna Yuh\0MaryAnn McCollough\0Derek Flickinger\0Jakub Michalak\0Thomas Huang\0Roy Wang\0Marge Lambrou\0Jacqueline Lamonna\0Yekesa Kosuru\0Amanda Slowikowski\0Nate Otten\0Sudhir Kondisetty\0Fleury\0Alexia Reyes\0Kamil Wos\0Joanna Michalak\0Peter Filice\0Ray Boppana\0Bhavya Sharma\0Hugo Steemers\0Keith Alexander\0Richard Par\0Bob & Camille Armantrout\0Alex Boden\0Seungmoon Song\0Kami Tomberlain\0Mihir Kulkarni\0Saurabh Misra\0Terrill, Darin & Nikita Frantz\0Kevin Dowling\0Heather Wainer\0The Maleys\0Jennifer Belardi\0Meng-Hao Chen\0Tyler Crummy\0Bharat Nain\0Philip Marquis\0Brian Macdonald\0James Doherty\0Matt Emerzian\0Raphael Ng\0M Shernell Smith\0Kristin Coverly\0Debra Kalister\0Jane Jeffers\0Grant Meadors\0Victoria Laskowski\0Kyle Cherry\0Cory Overturf\0Karan Talati\0Nicholas Wright\0Tommy Mullins\0Anthony Olivieri\0Payton Barnwell\0Jane Kim\0Gengxian Shi\0Sandeep Tamboli\0Chansidsimrun Gupta\0Kelsey Shimada\0William Hamilton\0Sanghak Shin\0Claire Connelly\0Deven Wagle\0Patrick Stakem\0Drew Bermudez\0Evren & Felix Herder\0Vinay Wagle\0Meena and Laksheta Yegnashankaran\0Sudeep Yegnashankaran\0Carla & Robert Bevins\0Eleanor Shuler\0Tahaseen Shaik\0Robert Zacharias\0Tess and J.R. Gaines\0Gayle and Towner Case\0Phyllis Njoroge\0Manju Amaresh\0Nicholas I Stefanov\0Emily Hickmott\0Amanda Vallon\0Punit Matta\0Ely Marsh\0Bryce Marsh\0Aidan Stanford\0Sun Min Kim\0Ashley Hedberg\0Logan Herman\0Lorelei Laird\0Kimberly, Michael & Noah Wong\0Megan Rabideau\0Mads McCart-Millner\0Allison Barker\0Chandler Dye\0Tara Watson\0Keith Sanders\0Diane Murray\0Christopher Kositzke\0Patty Acheson\0Courtney Pearson\0Lilly Colombo\0Bailey Colombo\0Caroline Colombo\0Andrew Colombo\0Alexander Colombo\0Elizabeth Colombo\0Ryan Colombo\0Vicky Colombo\0Sean Colombo\0Kathryn Colombo\0Anna Colombo\0William S. Colombo\0Luke Colombo\0Brian Hicks\0Win Halelamien\0michal zlamany\0MacKenzie Harper\0Choco Choi\0Seung Hee Han\0Jong Hyuk Choi\0Yoona Choi\0YI KE\0Beth & Ed Bunce\0Rosanne Barone\0Edward Jang\0Gabe Colombo\0Pranav Kumar\0William Colombo\0Sheila Colombo\0Heather Couture\0Martin Stolle\0Paul Keller\0Hsueh-Hao Chang\0ANAND JOGLEKAR\0Prasad Malvankar\0Gabriel Detter\0Tom Lauwers\0Carol Donahoe\0Sanjeev Koppal\0Victoria Martin\0Victor Akinwand\0Mark Maimone\0Mary Beth Brune\0Rohith Krishnan Pillai\0Nazanin Azimi\0Jiayu Bai\0Kushal Doshi\0Tejas Mathai\0Dayoung Lee\0Luke Yoder\0Joan Molin\0Jeanne Lu\0Dan & Kim Stefanov\0Michelle Mahouski\0Lisa & Dave Kayton\0Kelsey Kayton\0Man Jin\0Harrison Kayton\0Katie Coan\0Linda Hancock\0Sharon\0Maggie\0Linda\0Allison\0Donna\0Jean\0David\0Estelle\0Holly\0Krissy\0Grace\0Julie\0Anne\0Leslie\0Cathy\0Tim\0Lisa\0Cheri\0Danelle\0Jim\0Lilianna\0Atlas\0Joaquin\0Adam\0Jerry\0Drake\0Linus\0Molly\0Enna\0Aliza\0Elena\0Luna\0Finn\0Emmet\0Elliot\0Fitzpatrick\0Jason\0Livanya\0Jackson\0Kiran\0Lev\0Mollie\0Russell\0Mary\0Wyatt\0Maddox\0Alice\0Nevin\0Jacob\0Malina\0Paloma\0James\0Mila\0Cole\0Mateen\0Lila\0Andrew\0Kurtis\0Ezra\0Heloise\0Melodie\0Ewan\0Norah\0Seiichiro\0Masaki\0Madelyn\0Adele\0Aria\0August\0Advaith\0Felipe\0Louisa\0Zeynep\0Claire\0Lily\0Michael Bailey\0Declan Williams\0Jonathon Frigo\0Vaibhav Gupta\0Rukayat Muse-Ariyoh\0Thomas Park\0Lynne Wolf\0Nikolas Martelaro\0Andrew Gregory Miner Jr.\0Ayush Rungta\0Caroll Clausse\0Vasyl Lopushansky\0Rohil Bhargava\0Chien Chih Ho\0Spencer Barrett\0Julie Knight\0Bryan Nix\0Raghav Kapur\0Jordan Burger\0James Belliveau\0Qi Wang\0Chin Ling\0Brad Harkavy\0Sudhakar Maley\0Ching-Chun Yang\0Glenn Rossi\0Jaimin Shah\0Narahari Kondisetty\0Kit Needham\0Anne Gibbons\0Christian Romano\0Sonal Prakash Das\0Russell Kowalik\0Eric Rollins\0Jake Jones\0Debra Demchak\0Samuel Harbaugh\0Marc Zinck\0Andrew Moore\0Ryan Quigley\0Gus Birney\0Ephraim Birney\0Jackie Otto\0Vijay Adipudi\0Christian Knight\0Rishav Jain\0Andrew Weissman\0Clifford Huff\0Veronica Edwards\0Kyle Snyder\0Nicole Smith\0The Weinstein Family\0Robbee Kosak\0Don Earl\0Sam Childs\0Michael Sebetich\0Michelle Snyder\0Timothy McNulty\0Mary Soon Lee\0Rishikesh Vanarse\0Leondra Gonzalez\0Brian Huh\0Gursimran Singh\0Richard Guo\0Rishi Bhargava\0Dwayne Cales\0Maraika Lumholdt\0Anthony Matthews\0Gregory Rothauser\0Christine Kate Denucci\0Rahul Goel\0Katelyn Snyder\0Simi Raju\0Avneesh Khanna\0Paul Sabatino\0Lu Chen\0Rounak Baheti\0Griffin Tang\0Michele Gittleman\0Mark Tomczak\0Brian Lam\0Michael Stefanov\0Feraaz Hosein\0Brian Thornton\0Alina Sheykhet\0Harshvardhan Chunawala\0Information Networking Institute (INI)\0Sanae Minick\0Minwoo Choi\0Sang Hoon Lee\0Suguru Isoda\0Frances Lin\0Lance Miller\0Bradley Allen\0Sujit Raghavan\0Dhruv Agrawal\0Jeffrey Tone\0Philip Breitfeld\0Cynthia Reque Teran\0Chris Gassman\0Alexander Gutierrez\0Neal Bhasin\0Gabriel Zaragoza CMU SCS\0Lauren Jonak\0Dominic Jonak\0Rob Guzikowski\0Eli Slothower\0Ziqi Guo\0Liam Parry\0Ian Higgins\0Jose Vargas\0Desmond D'Souza\0Sean McGuire\0Yifan Su\0Astro Liz's Mema & Mike\0Edward Leventhal\0Minh Tran\0Elizabeth Norman\0Nathan Otten\0Viraj Pathak\0Vanessa D'Souza\0Monica D'Souza\0Christopher Canel\0Alexander Alvarez\0Kathryn Romeo\0Takafumi Shimazaki\0Karthik Ilango\0Sandhya Ilango\0William Lee-Moore\0Sidharth Sahoo\0Carrie Goldstein\0Susan Kreissman\0Varsha Kumar\0Ciara Figliuolo\0Matt Woodhead\0Yifan Xu\0Eduardo Frias\0David Wettergreen\0Stephanie Greene-Kroemer\0Kroemer Family\0Kurt Zimmerman\0Deepak Bapna\0Rakesh Bhargava\0Spencer Spiker\0Joshwa A. Raj (JTJR)\0Rita J. Raj (JTJR)\0Christopher Poole\0John Rushing\0Judy Ann Rushing\0Hunter Joshua Breau\0Elizabeth Rose Breau\0Tyler Robert Shaffer\0Mason John Shaffer\0Emma Grace Shaffer\0Bailey Renee Shaffer\0Junming Wang\0Ethan Decker\0Tiny Scoob\0Raewyn Arcadia Duvall\0Mary Duvall\0Dan Duvall\0Nathaniel Duvall\0Shelley Leger\0Chris Leger\0Kimberly Michelle Leger\0Christopher James Leger\0Jessica Caelyn Leger\0Katherine Alison Leger\0Elizabeth Virginia Leger\0Linnea Duvall\0Whitney Cushing\0Odesa Iliamna Duvall Cushing\0Potassium-9 (Kassy) Duvall\0Mishka Duvall\0Harper Wagner-Duvall\0Tsyganka Meka Duvall\0Albina Duvall\0Kuma Duvall\0Bonnie Blythe Duvall\0Phoebe Duvall\0Sammy Duvall\0Buffy Dog Leger\0Cesium Lewis Leger\0Mercury Wells Leger\0Gallium Inst of Tech Leger\0Molybdenum Tunnell Leger\0Rubidium Wilcox Leger\0Hydrogen Beam Piper Leger\0Actinium Clarke Leger\0Radium Heinlein Leger\0Iodine Asimov Leger\0Curium Kornbluth Leger\0Cadmium Simak Leger\0";

    // Longest Msg: 125char, count = 50. Messages longer than 99char, split into two.
    // All messages are in the pattern `{moc_crowdfund_name}\t{crowdfund_message}`
    const unsigned int LEN_MESSAGES = 3906; // 3859; // 2953; // 1156;
    const char MESSAGES[] =
        "Iris\tThanks to the generations of people who helped me get to the Moon!\0"
        ""
        "Rohil Bhargava\tShoot for the Moon, and even if you miss, you\'ll land among the stars - Les Brown\0"
        ""
        "Qi Wang\tGo with love and bravery from Rulian Pang and Ruiyi Wang.\0"
        ""
        "Brad Harkavy\tBrad, Rachael, Elizabeth, Keegan Harkavy & Ann Marie Mador\0"
        ""
        "Sudhakar Maley\tWe are proud of your accomplishments with IRIS and CMU Student Team Siri!\0"
        ""
        "Anne Gibbons\tIn honor of Dr. William E Gibbons.(1/2)\0"
        "Anne Gibbons\tLike those who built this Rover, Bill dared to dream. May his spirit rest among the stars.(2/2)\0"
        ""
        "Nagendranath Maley\tThank you Ragam Gadi Kondisetty Patel Maley Herr,(1/2)\0"
        "Nagendranath Maley\tFRC1640 PSU DASD, Mabaruma Annai Nyeri -- We Are!(2/2)\0"
        ""
        "Jake Jones\tDespite our failings, we are good hearted people\0"
        ""
        "Samuel Harbaugh\tThis craft represents the talent and dedication of the next generation of space roboticists.\0"
        ""
        "Marc Zinck\tMarc Zinck and Jean Marie Sloat\0"
        ""
        "Andrew Moore\tIn memory of Michael Arnold Moore: A well loved fellow, an adored Latin teacher and my father.\0"
        ""
        "Nicole Smith\tFor it is not difference which immobilizes us but silence,(1/2)\0"
        "Nicole Smith\t& there are so many silences to be broken(2/2)\0"
        ""
        "The Weinstein Family\tDear Man on the Moon-We hope you\'re cool with us crashing at your place.(1/2)\0"
        "The Weinstein Family\tWe promise not to wreck it!(2/2)\0"
        ""
        "Richard Par\tThe Moon draws us to its dark, basaltic seas,(1/2)\0"
        "Richard Par\tas we set sail deeper into our solar system. Ad lunam.(2/2)\0"
        ""
        "Don Earl\tDon Earl CFA BFA -68 | Teresa R. Earl | Jaime Earl Shroyer\0"
        ""
        "Sam Childs\tScientific Progress Goes 'Boink'?\0"
        ""
        "Mary Soon Lee\tAluminum, Al, 13 / This common metal / uncommonly convenient. / Foil. Cans. Apollo.\0"
        ""
        "Lu Chen\tCongratulations on your achievements and hope all our wishes will come true after making efforts!\0"
        ""
        "Michele Gittleman\tMichele, Miranda & Sage Gittleman-Arnold\0"
        ""
        "Mark Tomczak\t\'It was as if all of us, all over the world, had been given permission to dream big dreams.\' ~R.P.\0"
        ""
        "Brian Lam\tHi! Nihao! Hej! -the Lam fam\0"
        ""
        "Harshvardhan Chunawala\tSomeone someday will find this message & wonder how did it get here?~Harshvardhan Chunawala(1/2)\0"
        "Harshvardhan Chunawala\tINI MS32(2/2)\0"
        ""
        "Information Networking Institute (INI)\tWe heard there is no Wi-Fi on the Moon? INI invented it on Earth way back in 1989!Email ini@cmu.edu\0"
        ""
        "Iris\tNot anymore! This message will be sent to the Lander then to Earth from the Moon using Wi-Fi.\0"
        ""
        "Sujit Raghavan\tGreetings aliens, we come to you in peace. Please stop by Neil\'s house if you get a chance.\0"
        ""
        "Dhruv Agrawal\tDhruv and Polina\0"
        ""
        "Yifan Su\tTeleport successful!\0"
        ""
        "Alexander Alvarez\tNothing is given, earn your place. - Nicholas Alvarez, The Mini Moon Ranger Guy\0"
        ""
        "Takafumi Shimazaki\tCMU to the MOON!\0"
        ""
        "Karthik Ilango\tTo infinity and beyond!\0"
        ""
        "William Lee-Moore\tWith my thanks & love to Sora, Link, Sean, Emma, Nathan, Jacob, Sam, Peter, Lucy, Andrew, and Mary\0"
        ""
        "Susan Kreissman\tIn memory of all the children who have lost their fight with cancer\0"
        ""
        "Ciara Figliuolo\tMom/Grandma Rhonda, we love you to the moon and back! -Alicia, Scott, Quintyn, Zack, and Ciara\0"
        ""
        "John Hrivnak\tFor All Mankind\0"
        ""
        "Matt Woodhead\tHello, Moon! From the Woodhead Family: Mitchell R., Gordon A., Tracy L., & Matthew A. (GSIA \'95)\0"
        ""
        "Connor\t808 Alisa\0"
        ""
        "Stephanie Greene-Kroemer\tMoonlightDrownsOutAllButTheBrightestStars;(1/2)\0"
        "Stephanie Greene-Kroemer\tNothingIsWrittenInTheStars.NoOneControlsYourDestiny;Love,S(2/2)\0"
        ""
        "Kroemer Family\tDon\'t wobble or come closer, you are perfect where you are! Thanks for brightening up the darkness.\0"
        ""
        "Deepak Bapna\tRooting for Red & CMU. Honored to be part of this journey in initial phases. Dream does come true.\0"
        ""
        "Spencer Spiker\tFor my fallen comrades, Valentin Vassilev, Tom Balderson, & Todd Spiker, may you not be forgotten.\0"
        ""
        "Joshwa A. Raj (JTJR)\tRita & Joshwa Raj;Sharda & Amritlal Raja;(1/2)\0"
        "Joshwa A. Raj (JTJR)\tSanthana Mary;Thewaram Devasagayam;Nalini Raja;Peter Joshua(2/2)\0"
        ""
        "Christopher Poole\tOnly the first step\0"
        ""
        "Duo, The Duolingo Owl\t$DUOL\0";
} // namespace IrisNames
