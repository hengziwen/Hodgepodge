$path = "e:\Project\Git\Hodgepodge\Content\Main\Experiences\Exp_HodgeDefaultExperience.uasset"
$b = [System.IO.File]::ReadAllBytes($path)
$s = [System.Text.Encoding]::ASCII.GetString($b, 0, [Math]::Min(4000, $b.Length))
$m = [regex]::Matches($s, '[ -~]{4,}')
$i = 0
foreach ($x in $m) {
    $i++
    if ($i -le 60) { Write-Output $x.Value }
}
