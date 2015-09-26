'use strict';

var eslint = require('eslint');

/**
 * JavaScript files
 */
var JS_FILES = ['app.js'];

/**
 * Files to clean
 */
var CLEAN_FILES = ['eslint.xml'];

module.exports = function(grunt) {
    // Project configuration.
    grunt.initConfig({
        pkg: grunt.file.readJSON('package.json'),
        clean: {
            all: CLEAN_FILES
        },
        eslint: {
            options: {
                callback: function(response) {
                    var CLIEngine = require('eslint').CLIEngine;
                    eslint = new CLIEngine();

                    // save the output to eslint.xml in jslint XML format
                    var formatter = eslint.getFormatter('jslint-xml');
                    var report = formatter(response.results);
                    grunt.file.write('eslint.xml', report);

                    // also output to the console
                    formatter = eslint.getFormatter('stylish');
                    report = formatter(response.results);
                    console.log('eslint.xml', report);
                },
                'silent': true
            },
            all: JS_FILES
        },
    });

    //
    // Plugins
    //
    grunt.loadNpmTasks('grunt-contrib-clean');
    grunt.loadNpmTasks('gruntify-eslint');

    //
    // Tasks
    //
    grunt.registerTask('lint', ['eslint']);
    grunt.registerTask('default', ['all']);
    grunt.registerTask('all', ['clean', 'lint']);
};
