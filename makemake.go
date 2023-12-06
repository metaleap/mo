package main

import (
	"io/fs"
	"os"
	"path/filepath"
	"slices"
	"strings"
)

const makefileHead = `# DO NOT EDIT: auto-generated by makemake.go via build_*.sh

CXX = g++
CXXFLAGS = --debug -std=c++20 -march=native


`

var (
	strEnds      = strings.HasSuffix
	strBegins    = strings.HasPrefix
	everLibNames = map[string][]string{
		"mo2d": {"sfml-graphics", "sfml-window", "sfml-system", "GL"},
	}
)

type makeRule struct {
	outPath  string
	inPath   string
	depPaths []string
}

func main() {
	executable_names := os.Args[1:]
	var lib_dep_names []string
	cur_dir_path := fsAbs(".")
	var all_source_file_paths []string
	fs.WalkDir(os.DirFS(cur_dir_path), ".", func(fsPath string, fsEntry fs.DirEntry, err error) error {
		if err != nil {
			panic(err)
		}
		if fsEntry.IsDir() && strBegins(fsPath, "libdeps/") && (1 == strings.Count(fsPath, "/")) {
			lib_dep_names = append(lib_dep_names, filepath.Base(fsPath))
		}
		if !strBegins(fsPath, "mo") {
			return nil
		}
		if (!fsEntry.IsDir()) && (strEnds(fsPath, ".cpp") || strEnds(fsPath, ".h")) {
			all_source_file_paths = append(all_source_file_paths, fsPath)
		}
		return nil
	})

	var rules []makeRule
	for _, source_file_path := range all_source_file_paths {
		if strEnds(source_file_path, ".cpp") {
			rule := makeRule{inPath: source_file_path, outPath: fsCppPathToObjPath(source_file_path)}
			src := fsRead(rule.inPath)
			for again, needle_incl := true, "#include \"."; again; {
				idx_incl := strings.LastIndex(src, needle_incl)
				if again = (idx_incl >= 0); again {
					idx_off := idx_incl + len(needle_incl) - len(".")
					idx_quot := strings.IndexByte(src[idx_off:], '"')
					if again, idx_quot = (idx_quot > 0), idx_off+idx_quot; again {
						h_file_path := filepath.Join(filepath.Dir(rule.inPath), src[idx_off:idx_quot])
						cpp_file_path := h_file_path[:len(h_file_path)-len(".h")] + ".cpp"
						for _, src_file_path := range []string{h_file_path, cpp_file_path} {
							if fsIsFile(src_file_path) && slices.Index(rule.depPaths, src_file_path) < 0 {
								rule.depPaths = append(rule.depPaths, src_file_path)
							}
						}
						src = src[:idx_incl]
					}
				}
			}
			rules = append(rules, rule)
		}
	}

	var buf strings.Builder
	buf.WriteString(makefileHead)
	for _, rule := range rules {
		for _, executable_name := range executable_names {
			if rule.outPath == "bin/"+executable_name+"_main.o" {
				obj_file_paths, is_first := map[string]bool{}, true
				gatherObjFileNamesFor(rules, obj_file_paths, rule.outPath)

				buf.WriteString("bin/" + executable_name)
				for obj_file_path := range obj_file_paths {
					buf.WriteString(iIf(is_first, ": ", " ") + obj_file_path)
					is_first = false
				}
				buf.WriteByte('\n')
				buf.WriteString("\t$(CXX) $(CXXFLAGS) -Lbin")
				for _, lib_name := range everLibNames[executable_name] {
					buf.WriteString(" -l" + lib_name)
				}
				for _, lib_name := range lib_dep_names {
					buf.WriteString(" -l" + lib_name)
				}
				for obj_file_path := range obj_file_paths {
					buf.WriteString(" " + obj_file_path)
				}
				buf.WriteString(" -o bin/" + executable_name + "\n")
				buf.WriteByte('\n')
				break
			}
		}
	}
	{
		buf.WriteString("clean:\n\trm -f bin/*.o\n")
		for _, executable_name := range executable_names {
			buf.WriteString("\trm -f bin/" + executable_name + "\n")
		}
		buf.WriteString("# NOTE on clean: all bin/*.so files _stay_! they're rarely or never updated 3rd-party deps.\n\n")
		buf.WriteByte('\n')
	}
	for _, rule := range rules {
		buf.WriteString(rule.outPath + ": " + rule.inPath)
		for _, dep := range rule.depPaths {
			buf.WriteString(" " + dep)
		}
		buf.WriteString("\n\t$(CXX) -c $(CXXFLAGS) -Ilibdeps/" + strings.Join(lib_dep_names, " -Ilibdeps/") + " " + rule.inPath + " -o " + rule.outPath + "\n")
		buf.WriteByte('\n')
	}
	fsWrite("makefile", buf.String())
}

func iIf[T any](b bool, t T, f T) T {
	if b {
		return t
	}
	return f
}

func gatherObjFileNamesFor(rules []makeRule, into map[string]bool, forObjFilePath string) {
	if into[forObjFilePath] {
		return
	}
	into[forObjFilePath] = true
	idx_rule := slices.IndexFunc(rules, func(it makeRule) bool { return (it.outPath == forObjFilePath) })
	if idx_rule > 0 {
		for _, dep_path := range rules[idx_rule].depPaths {
			if strEnds(dep_path, ".cpp") {
				gatherObjFileNamesFor(rules, into, fsCppPathToObjPath(dep_path))
			}
		}
	}
}

func fsCppPathToObjPath(fsPath string) string {
	return "bin/" + fsCppPathToObjName(fsPath) + ".o"
}

func fsCppPathToObjName(fsPath string) string {
	idx := strings.LastIndexByte(fsPath, '.')
	return strings.ReplaceAll(fsPath[:idx], "/", "_")
}

func fsIsFile(fsPath string) bool {
	stat, err := os.Stat(fsPath)
	return (err == nil) && (stat != nil) && !stat.IsDir()
}

func fsRead(fsPath string) string {
	raw, err := os.ReadFile(fsPath)
	if err != nil {
		panic(err)
	}
	return string(raw)
}

func fsWrite(fsPath string, src string) {
	err := os.WriteFile(fsPath, []byte(src), os.ModePerm)
	if err != nil {
		panic(err)
	}
}

func fsAbs(fsPath string) string {
	ret, err := filepath.Abs(fsPath)
	if err != nil {
		panic(err)
	}
	return ret
}
