#!/usr/bin/perl 

if ($ENV{'REQUEST_METHOD'} eq "POST" ) 
{
	# check for upload file
	read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});
    $Recu="STDIN (Methode POST)" 
}
elsif ($ENV{'REQUEST_METHOD'} eq "DELETE")
{
	# TODO: delete content
}
else {
    $Recu="QUERY_STRING (Methode GET)";
    $buffer = $ENV{'QUERY_STRING'};
}
# Traitement et découpage.
    @pairs = split(/&/, $buffer);
    foreach $pair (@pairs) {
        ($name, $value) = split(/=/, $pair);
        $value =~ tr/+/ /;
        $value =~ s/%(..)/pack("C", hex($1))/eg;
        $FORM{$name} = $value;
}

print STDOUT "Content-Type: text/html\r\n";
print STDOUT "\r\n";
print STDOUT "<html lang=\"fr\"><head><meta charset=\"UTF-8\" />";
print STDOUT "<title>Resultat</title></head>\n";
print STDOUT "<body bgcolor=\"#FFFFFF\">\n";

print STDOUT "<h1>Résultat du traitement du formulaire</h1>\n";
print STDOUT "<h2>Chaine de données reçue par le CGI</h2>\n";
print STDOUT "$Recu <b>$buffer</b>\n";

print STDOUT "<h2>Liste des informations décodées</h2>\n";
print STDOUT "<ul>\n";

#%getquery = &get_query_string; 

#&cgi_response_header;
#print "Form : ".$getquery{'form'}."<br>\n";
#print "Goal : "."POST??"."<br>\n"; # TODO: READ STDIN comme ici: https://fr.acervolima.com/perl-get-vs-post-en-cgi/ (je te laisse faire)
#print "Submit : "."POST??"."<br>\n";

#print "<pre>\n";

foreach $match (keys (%FORM)) {
    print STDOUT "<li><b>$match: </b>".$FORM{$match};
}

print STDOUT "</ul>\n";
print STDOUT "</body></html>\n";
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
 print "CONTENT_LENGTH : ".$ENV{'CONTENT_LENGTH'}."<br>\n";
# print "HTTP_FROM : ".$ENV{'HTTP_FROM'}."<br>\n";
# print "HTTP_ACCEPT : ".$ENV{'HTTP_ACCEPT'}."<br>\n";
# print "HTTP_USER_AGENT : ".$ENV{'HTTP_USER_AGENT'}."<br>\n";
# print "HTTP_REFERER : ".$ENV{'HTTP_REFERER'}."<br>\n";

sub	cgi_response_header
{
	# Use a partial custom header functionnality
	print "Content-Type: text/html\r\n";
	print "\r\n";



	#print $ENV{'SERVER_PROTOCOL'}." 200 OK\r\n"; # voir par la suite pour passer la rep en arg de la fn
	# la date ca peut fonctionner comme ca: soit on `date` et on formatte la sortie de la cmd
	#print "Date: ".`date`."\r\n"; # Pas le bon format
	# soit on utilise la facon perl (perldoc perfunc -> chercher localtime)
	# on est dans un contexte de liste:
	# ($sec, $min, $hour, $day_of_month, $month, $years_since_1900, $day_of_week, $day_of_year, $is_winter_time)=localtime;
	#print $day_of_month." / ".$month; # par exemple
	#print "Server: ".$ENV{'SERVER_SOFTWARE'}."\r\n";
	#print "Cache-Control: no-cache\r\n";
	#print "Content_length : ".$ENV{'CONTENT_LENGTH'}."\n"; # Pas obligatoire, vaut mieux rien mettre
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
