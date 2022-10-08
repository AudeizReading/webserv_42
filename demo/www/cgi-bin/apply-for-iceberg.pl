#!/usr/bin/perl 

#print "content-type: text/html\n\n"; 

%httpform = &get_query_string; 

&cgi_response_header;
print "Form : ".$httpform{'form'}."<br>\n";
print "Goal : ".$httpform{'goal'}."<br>\n";
print "Submit : ".$httpform{'submit'}."<br>\n";

#
print "GATEWAY_INTERFACE : ".$ENV{'GATEWAY_INTERFACE'}."<br>\n";
print "SERVER_NAME : ".$ENV{'SERVER_NAME'}."<br>\n";
print "SERVER_SOFTWARE : ".$ENV{'SERVER_SOFTWARE'}."<br>\n";
print "SERVER_PROTOCOL : ".$ENV{'SERVER_PROTOCOL'}."<br>\n";
print "SERVER_PORT : ".$ENV{'SERVER_PORT'}."<br>\n";
print "REQUEST_METHOD : ".$ENV{'REQUEST_METHOD'}."<br>\n";
print "PATH_INFO : ".$ENV{'PATH_INFO'}."<br>\n";
print "PATH_TRANSLATED : ".$ENV{'PATH_TRANSLATED'}."<br>\n";
print "SCRIPT_NAME : ".$ENV{'SCRIPT_NAME'}."<br>\n";
print "DOCUMENT_ROOT : ".$ENV{'DOCUMENT_ROOT'}."<br>\n";
print "QUERY_STRING : ".$ENV{'QUERY_STRING'}."<br>\n";
print "REMOTE_HOST : ".$ENV{'REMOTE_HOST'}."<br>\n";
print "REMOTE_ADDR : ".$ENV{'REMOTE_ADDR'}."<br>\n";
print "REMOTE_USER : ".$ENV{'REMOTE_USER'}."<br>\n";
print "REMOTE_IDENT : ".$ENV{'REMOTE_IDENT'}."<br>\n";
print "AUTH_TYPE : ".$ENV{'AUTH_TYPE'}."<br>\n";
print "CONTENT_TYPE : ".$ENV{'CONTENT_TYPE'}."<br>\n";
print "CONTENT_LENGTH : ".$ENV{'CONTENT_LENGTH'}."<br>\n";
print "HTTP_FROM : ".$ENV{'HTTP_FROM'}."<br>\n";
print "HTTP_ACCEPT : ".$ENV{'HTTP_ACCEPT'}."<br>\n";
print "HTTP_USER_AGENT : ".$ENV{'HTTP_USER_AGENT'}."<br>\n";
print "HTTP_REFERER : ".$ENV{'HTTP_REFERER'}."<br>\n";

sub	cgi_response_header
{
	print $ENV{'SERVER_PROTOCOL'}." 200 OK\n"; # voir par la suite pour passer la rep en arg de la fn
	print "Date: ".`date`;
	print "Server: ".$ENV{'SERVER_SOFTWARE'}."\n";
	print "Last-Modified: <derniere date de modif du doc auquel on tente d'acceder -> c'est pour la gestion de cache\n";
	print "ETag: <entity tag> ->un descripteur d'entité donne une ref unique pr cette ressource, fournit un moyen pr cacher les docs\n";
	print "Accept-Ranges : bytes\n";
	print "Content_length : ".$ENV{'CONTENT_LENGTH'}."\n";
	print "Connection : close\n";
	print "Content_type :text/html\n";
}

sub get_query_string
{
	local ($buffer, @pairs, $pair, $name, $value, %form);
	$buffer = $env{'query_string'};
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
