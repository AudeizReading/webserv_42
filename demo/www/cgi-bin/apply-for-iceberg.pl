#!/usr/bin/perl 

print "content-type: text/html\n\n"; 

%httpform = &get_query_string; 

print "Form : ".$httpform{'form'}."<br>\n";
print "Goal : ".$httpform{'goal'}."<br>\n";
print "Submit : ".$httpform{'submit'}."<br>\n";
print "CGI_TEST : ".$ENV{'CGI_TEST'}."<br>\n";

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
