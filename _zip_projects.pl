# (c)2013 Dr. Aleksander Malinowski http://gdansk.bradley.edu/olekmali/
# all rights reserved, as is, use at your own risk, no claims to any usefulness
# not responsible for any loses or malfunction,  available under GPL license
use strict;

my $ZIP = "C:\\Program Files\\ZTreeWin\\PKZIP25.exe";

# read the folder contents
opendir(DIR, ".") or return;
my @filelist=readdir(DIR);
closedir(DIR);

my $item;
foreach $item (@filelist)
{
    next if (($item eq ".") or ($item eq "..") or ($item eq ".git") or ($item =~ /_done$/i));
    next unless (-d $item);
    next if ($item =~ /_todo_/i);
    
    my $file = uc($item);
    my $command = "\"$ZIP\" -add=update -recurse -dir=current \"$file\" \"$item\\*.*\"";
    print "$command\n";
    system($command);
}