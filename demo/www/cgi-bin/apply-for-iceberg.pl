#!/usr/bin/perl 

if ($ENV{'REQUEST_METHOD'} eq "POST" ) 
{
	# check for upload file
	if ($ENV{'CONTENT_LENGTH'} > 0)
	{
		read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});
		$POST="STDIN (Methode POST)" ;
		%FORM=&split_cgi_array($buffer);
	}
    $buffer = $ENV{'QUERY_STRING'};
}
elsif ($ENV{'REQUEST_METHOD'} eq "DELETE")
{
	# TODO: delete content
}
elsif ($ENV{'REQUEST_METHOD'} eq "GET")
{
    $POST="QUERY_STRING (Methode GET)";
    $buffer = $ENV{'QUERY_STRING'};
}
else {
	print "Content-Type: text/html\r\n";
	print $ENV{'SERVER_PROTOCOL'}." 403 Forbidden\r\n"; 
	print STDOUT "\r\n";
	exit 1;
}

# split by & and =, put this in a sort of map
sub split_cgi_array
{
	local (@pairs, $pair, $name, $value, %form, $arg);
	($arg)=@_;
	print "arg\r\n:".$arg."\nfin arg\n";
	@pairs = split(/&/, $buffer);
    foreach $pair (@pairs) {
        ($name, $value) = split(/=/, $pair);
        $value =~ tr/+/ /;
        $value =~ s/%(..)/pack("C", hex($1))/eg;
        $form{$name} = $value;
	}
	%form;
}
#%GETQUERY = &get_query_string;
%GETQUERY = &split_cgi_array($buffer);

print STDOUT "Content-Type: text/html\r\n";
print STDOUT "\r\n";
print STDOUT "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n";
print STDOUT "<html lang=\"fr\">\r\n";
print STDOUT "<head>\r\n";
print STDOUT "\t<meta charset=\"UTF-8\">\r\n";
print STDOUT "\t<title>Resultat</title>\r\n";
print STDOUT "</head>\r\n";
print STDOUT "<body bgcolor=\"#FFBFFF\">\r\n";

print STDOUT "\t<h1>Résultat du traitement du formulaire</h1>\r\n";
print STDOUT "\t<h2>Chaine de données reçue par le CGI</h2>\r\n";
print STDOUT "\t<p>$POST <b>$buffer</b></p>\r\n";

print STDOUT "\t<h2>Liste des informations décodées</h2>\r\n";
print STDOUT "\t<ul>\r\n";

foreach $match (keys (%GETQUERY)) {
    print STDOUT "\t\t<li><b>$match: </b>".$GETQUERY{$match}."</li>\n";
}
foreach $match (keys (%FORM)) {
    print STDOUT "\t\t<li><b>$match: </b>".$FORM{$match}."</li>\n";
}

print STDOUT "\t</ul>\r\n";
print STDOUT "\t<p>Come back at index.html? <a href=\"../index.html\">Click Here:</a> </p>\r\n";
print STDOUT "</body>\r\n</html>\r\n";

sub cgi_print_environnement
{
	print STDOUT "\t<ul>\r\n";
	print STDOUT "\t\t<span>ENVIRONNEMENT:</span>\r\n";
	foreach $env (keys (%ENV))
	{
		print STDOUT "\t\t<li><b>$env: </b>".$ENV{$env}."</li>\n";
	}
	print STDOUT "\t</ul>\r\n";
}

#&cgi_print_environnement();

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
