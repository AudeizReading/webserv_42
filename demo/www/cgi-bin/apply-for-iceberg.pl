#!/usr/bin/perl 

#%getquery = &get_query_string; 

#&cgi_response_header;
#print "Form : ".$getquery{'form'}."<br>\n";
#print "Goal : "."POST??"."<br>\n"; # TODO: READ STDIN comme ici: https://fr.acervolima.com/perl-get-vs-post-en-cgi/ (je te laisse faire)
#print "Submit : "."POST??"."<br>\n";

#print "<pre>\n";

#foreach $key (sort keys(%ENV)) {
#print "$key = $ENV{$key}\r\n";
#}
#print "</pre>\n";

print "Bla bla bla bla\n";
print <STDOUT>;
#print <STDIN>;
#
# print "GATEWAY_INTERFACE : ".$ENV{'GATEWAY_INTERFACE'}."<br>\n";
# print "SERVER_NAME : ".$ENV{'SERVER_NAME'}."<br>\n";
# print "SERVER_SOFTWARE : ".$ENV{'SERVER_SOFTWARE'}."<br>\n";
# print "SERVER_PROTOCOL : ".$ENV{'SERVER_PROTOCOL'}."<br>\n";
# print "SERVER_PORT : ".$ENV{'SERVER_PORT'}."<br>\n";
# print "REQUEST_METHOD : ".$ENV{'REQUEST_METHOD'}."<br>\n";
# print "PATH_INFO : ".$ENV{'PATH_INFO'}."<br>\n";
# print "PATH_TRANSLATED : ".$ENV{'PATH_TRANSLATED'}."<br>\n";
# print "SCRIPT_NAME : ".$ENV{'SCRIPT_NAME'}."<br>\n";
# print "DOCUMENT_ROOT : ".$ENV{'DOCUMENT_ROOT'}."<br>\n";
# print "QUERY_STRING : ".$ENV{'QUERY_STRING'}."<br>\n";
# print "REMOTE_HOST : ".$ENV{'REMOTE_HOST'}."<br>\n";
# print "REMOTE_ADDR : ".$ENV{'REMOTE_ADDR'}."<br>\n";
# print "REMOTE_USER : ".$ENV{'REMOTE_USER'}."<br>\n";
# print "REMOTE_IDENT : ".$ENV{'REMOTE_IDENT'}."<br>\n";
# print "AUTH_TYPE : ".$ENV{'AUTH_TYPE'}."<br>\n";
# print "CONTENT_TYPE : ".$ENV{'CONTENT_TYPE'}."<br>\n";
# print "CONTENT_LENGTH : ".$ENV{'CONTENT_LENGTH'}."<br>\n";
# print "HTTP_FROM : ".$ENV{'HTTP_FROM'}."<br>\n";
# print "HTTP_ACCEPT : ".$ENV{'HTTP_ACCEPT'}."<br>\n";
# print "HTTP_USER_AGENT : ".$ENV{'HTTP_USER_AGENT'}."<br>\n";
# print "HTTP_REFERER : ".$ENV{'HTTP_REFERER'}."<br>\n";

sub	cgi_response_header
{
	print $ENV{'SERVER_PROTOCOL'}." 200 OK\r\n"; # voir par la suite pour passer la rep en arg de la fn
	# la date ca peut fonctionner comme ca: soit on `date` et on formatte la sortie de la cmd
	#print "Date: ".`date`."\r\n"; # Pas le bon format
	# soit on utilise la facon perl (perldoc perfunc -> chercher localtime)
	# on est dans un contexte de liste:
	# ($sec, $min, $hour, $day_of_month, $month, $years_since_1900, $day_of_week, $day_of_year, $is_winter_time)=localtime;
	# print $day_of_month." / ".$month; # par exemple
	#print "Server: ".$ENV{'SERVER_SOFTWARE'}."\r\n";
	print "Cache-Control: no-cache\r\n";
	print "Content-Type: text/html\r\n";
	#print "Content_length : ".$ENV{'CONTENT_LENGTH'}."\n"; # Pas obligatoire, vaut mieux rien mettre
	print "\r\n";
}

sub get_query_string
{
	local ($buffer, @pairs, $pair, $name, $value, %form);
	$buffer = $ENV{'QUERY_STRING'};
	@pairs = split(/&/, $buffer);
	foreach $pair (@pairs)
	{
		($name, $value) = split(/=/, $pair);
		$value =~ tr/+/ /;
		$value =~ s/%(..)/pack("c", hex($1))/eg;
		$form{$name} = $value;
	}
	%form;
}
