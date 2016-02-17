% Script for compiling documentation using m2html

cd ..; 
% recursively gather a list of folders containing code (ignore external and doc folders)
[s,dirs]=unix('find . -type d | sed ''/.svn/d'' | sed ''/external/d'' | sed ''/doc/d''');
% split into a cell array for m2html
tmp=regexp(dirs, '[\f\n\r]', 'split');
for i=2:size(tmp,2)-1,dirc{i-1}=tmp{i};end
% add m2html to search path
addpath('external/m2html');
% generate docs
m2html('mfiles',dirc,'htmldir','doc/m2html','todo','on','graph','on','globalHypertextLinks','on','indexFile','index','recursive','on');
